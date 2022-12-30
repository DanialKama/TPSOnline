// Copyright 2022 Danial Kamali. All Rights Reserved.

#include "ProjectileActor.h"

#include "Engine/DataTable.h"
#include "GameFramework/ProjectileMovementComponent.h"
#include "Kismet/GameplayStatics.h"
#include "Kismet/KismetMathLibrary.h"
#include "Particles/ParticleSystemComponent.h"
#include "Sound/SoundCue.h"
#include "Structures/ExplosiveProjectileInfoStruct.h"
#include "Structures/ProjectileInfoStruct.h"

AProjectileActor::AProjectileActor()
{
	PrimaryActorTick.bCanEverTick = false;
	PrimaryActorTick.bStartWithTickEnabled = false;

	StaticMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("Static Mesh"));
	RootComponent = StaticMesh;
	StaticMesh->SetComponentTickEnabled(false);
	StaticMesh->SetNotifyRigidBodyCollision(true);
	StaticMesh->CanCharacterStepUpOn = ECB_No;
	StaticMesh->SetCollisionProfileName("Projectile");
	StaticMesh->SetGenerateOverlapEvents(false);
	StaticMesh->bReturnMaterialOnMove = true;

	TrailParticle = CreateDefaultSubobject<UParticleSystemComponent>(TEXT("Trail Particle"));
	TrailParticle->SetupAttachment(StaticMesh, TEXT("TrailSocket"));

	ProjectileMovement = CreateDefaultSubobject<UProjectileMovementComponent>(TEXT("Projectile Movement"));
	ProjectileMovement->InitialSpeed = 5000.0f;
	ProjectileMovement->MaxSpeed = 5000.0f;
	ProjectileMovement->ProjectileGravityScale = 0.1f;
	
	// Load data tables
	static ConstructorHelpers::FObjectFinder<UDataTable> ProjectileDataObject(TEXT("DataTable'/Game/Blueprints/Projectiles/DT_ProjectileInfo.DT_ProjectileInfo'"));
	if (ProjectileDataObject.Succeeded())
	{
		ProjectileDataTable = ProjectileDataObject.Object;
	}

	static ConstructorHelpers::FObjectFinder<UDataTable> ExplosiveProjectileDataObject(TEXT("DataTable'/Game/Blueprints/Projectiles/DT_ExplosiveProjectileInfo.DT_ExplosiveProjectileInfo'"));
	if (ProjectileDataObject.Succeeded())
	{
		ExplosiveProjectileDataTable = ExplosiveProjectileDataObject.Object;
	}

	// Initialize variables
	AmmoType = EAmmoType::FiveFiveSix;
	bIsExplosive = false;
	NumberOfPellets = 1;
	PelletSpread = 0.0f;
	SwitchExpression = 0;
	LifeSpan = 2.0f;
}

void AProjectileActor::BeginPlay()
{
	Super::BeginPlay();

	if (GetLocalRole() == ROLE_Authority)
	{
		StaticMesh->OnComponentHit.AddDynamic(this, &AProjectileActor::OnHit);
	}

	// TODO: don't set the life span like this
	SetLifeSpan(LifeSpan);
}

void AProjectileActor::OnHit(UPrimitiveComponent* HitComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, FVector NormalImpulse, const FHitResult& Hit)
{
	if (Hit.PhysMaterial.IsValid())
	{
		// Calculating it once and using it many times
		SwitchExpression = StaticEnum<EPhysicalSurface>()->GetIndexByValue(UGameplayStatics::GetSurfaceType(Hit));
	}
	
	const FName AmmoName = StaticEnum<EAmmoType>()->GetValueAsName(AmmoType);
	if (bIsExplosive && ExplosiveProjectileDataTable)
	{
		if (const FExplosiveProjectileInfo* ExplosiveProjectileInfo =
			ExplosiveProjectileDataTable->FindRow<FExplosiveProjectileInfo>(AmmoName, TEXT("Projectile Info Context"), true))
		{
			// Apply radial damage with fall off for explosive projectiles
			const TArray<AActor*> IgnoreActors;
			UGameplayStatics::ApplyRadialDamageWithFalloff(GetWorld(), ExplosiveProjectileInfo->BaseDamage, ExplosiveProjectileInfo->MinimumDamage, Hit.ImpactPoint, ExplosiveProjectileInfo->DamageInnerRadius, ExplosiveProjectileInfo->DamageOuterRadius, 2.0f, DamageType, IgnoreActors, GetOwner(), GetInstigatorController(), ECollisionChannel::ECC_Visibility);
		}
	}
	else if (ProjectileDataTable)
	{
		if (const FProjectileInfo* ProjectileInfo =
			ProjectileDataTable->FindRow<FProjectileInfo>(AmmoName, TEXT("Projectile Info Context"), true))
		{
			// Apply point damage for nonexplosive projectiles based on surface type
			UGameplayStatics::ApplyPointDamage(Hit.GetActor(), CalculatePointDamage(ProjectileInfo), Hit.TraceStart, Hit, GetInstigatorController(), GetOwner(), DamageType);
		}
	}
	
	MulticastHitEffects(SwitchExpression, Hit);
	Destroy();
}

float AProjectileActor::CalculatePointDamage(const FProjectileInfo* ProjectileInfo) const
{
	switch (SwitchExpression)
	{
	case 1:
		// Head
		return ProjectileInfo->DamageToHead;
	case 2:
		// Body
		return ProjectileInfo->DamageToBody;
	case 3:
		// Arm
		return ProjectileInfo->DamageToArm;
	case 4:
		// Leg
		return ProjectileInfo->DamageToLeg;
	case 0: default:
		return ProjectileInfo->DefaultDamage;
	}
}

void AProjectileActor::MulticastHitEffects_Implementation(uint8 InSwitchExpression, FHitResult HitResult)
{
	UParticleSystem* Emitter;
	USoundCue* Sound;
	UMaterialInterface* Decal;
	FVector DecalSize;
	float DecalLifeSpan;
	FindHitEffects(InSwitchExpression, Emitter, Sound, Decal, DecalSize, DecalLifeSpan);

	const FRotator SpawnRotation = UKismetMathLibrary::MakeRotFromX(HitResult.ImpactNormal);
	
	// Spawn impact emitter
	UGameplayStatics::SpawnEmitterAtLocation(GetWorld(), Emitter, HitResult.ImpactPoint, SpawnRotation);
	
	// Spawn decal attached to hit component
	UGameplayStatics::SpawnDecalAttached(Decal, DecalSize, HitResult.GetComponent(), HitResult.BoneName, HitResult.ImpactPoint,
		SpawnRotation, EAttachLocation::KeepWorldPosition, DecalLifeSpan);

	// Play sound at the impact location
	UGameplayStatics::PlaySoundAtLocation(GetWorld(), Sound, HitResult.ImpactPoint);

	// If the projectile is explosive in addition to the surface impact emitter another emitter spawn for the explosion
	if (bIsExplosive)
	{
		UGameplayStatics::SpawnEmitterAtLocation(GetWorld(), HitEffects.ExplosiveEmitter, HitResult.ImpactPoint, SpawnRotation);
	}
}

void AProjectileActor::FindHitEffects(uint8 InSwitchExpression, UParticleSystem*& Emitter, USoundCue*& Sound, UMaterialInterface*& Decal, FVector& DecalSize, float& DecalLifeSpan) const
{
	switch (InSwitchExpression)
	{
	case 1: case 2: case 3: case 4:
		// Head, Body, Arm, Leg
		Emitter			= HitEffects.FleshHitEmitter;
		Sound			= HitEffects.FleshHitSound;
		Decal			= HitEffects.FleshDecal;
		DecalSize		= HitEffects.FleshDecalSize;
		DecalLifeSpan	= HitEffects.FleshDecalLifeSpan;
		break;
	case 5:
		// Stone
		Emitter			= HitEffects.StoneHitEmitter;
		Sound			= HitEffects.ObjectHitSound;
		Decal			= HitEffects.StoneDecal;
		DecalSize		= HitEffects.ObjectDecalSize;
		DecalLifeSpan	= HitEffects.ObjectDecalLifeSpan;
		break;
	case 6:
		// Metal
		Emitter			= HitEffects.MetalHitEmitter;
		Sound			= HitEffects.ObjectHitSound;
		Decal			= HitEffects.MetalDecal;
		DecalSize		= HitEffects.ObjectDecalSize;
		DecalLifeSpan	= HitEffects.ObjectDecalLifeSpan;
		break;
	case 7:
		// Wood
		Emitter			= HitEffects.WoodHitEmitter;
		Sound			= HitEffects.ObjectHitSound;
		Decal			= HitEffects.WoodDecal;
		DecalSize		= HitEffects.ObjectDecalSize;
		DecalLifeSpan	= HitEffects.ObjectDecalLifeSpan;
		break;
	case 0: default:
		Emitter			= HitEffects.StoneHitEmitter;
		Sound			= HitEffects.ObjectHitSound;
		Decal			= HitEffects.StoneDecal;
		DecalSize		= HitEffects.ObjectDecalSize;
		DecalLifeSpan	= HitEffects.ObjectDecalLifeSpan;
	}
}
