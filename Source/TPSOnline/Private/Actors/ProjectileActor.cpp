// All Rights Reserved.

#include "Actors/ProjectileActor.h"
#include "GameFramework/ProjectileMovementComponent.h"
#include "Kismet/GameplayStatics.h"
#include "Particles/ParticleSystemComponent.h"

AProjectileActor::AProjectileActor()
{
	PrimaryActorTick.bCanEverTick = false;

	StaticMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("Static Mesh"));
	SetRootComponent(StaticMesh);
	StaticMesh->SetComponentTickEnabled(false);
	StaticMesh->SetNotifyRigidBodyCollision(true);
	StaticMesh->CanCharacterStepUpOn = ECB_No;
	StaticMesh->SetCollisionProfileName("Projectile");
	StaticMesh->SetGenerateOverlapEvents(false);
	StaticMesh->bReturnMaterialOnMove = true;
	StaticMesh->OnComponentHit.AddDynamic(this, &AProjectileActor::OnHit);

	TrailParticle = CreateDefaultSubobject<UParticleSystemComponent>(TEXT("Trail Particle"));
	TrailParticle->SetupAttachment(StaticMesh, TEXT("TrailSocket"));

	ProjectileMovement = CreateDefaultSubobject<UProjectileMovementComponent>(TEXT("Projectile Movement"));
	ProjectileMovement->InitialSpeed = 5000.0f;
	ProjectileMovement->MaxSpeed = 5000.0f;
	ProjectileMovement->ProjectileGravityScale = 0.1f;

	// Initialize variables
	AmmoType = EAmmoType::FiveFiveSix;
	bIsExplosive = false;
	NumberOfPellets = 1;
	PelletSpread = 0.0f;
	LifeSpan = 2.0f;
}

void AProjectileActor::BeginPlay()
{
	Super::BeginPlay();

	SetLifeSpan(LifeSpan);
}

void AProjectileActor::OnHit(UPrimitiveComponent* HitComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, FVector NormalImpulse, const FHitResult& Hit)
{
	if (GetLocalRole() == ROLE_Authority)
	{
		if (bIsExplosive)
		{
			
		}
		else
		{
			
		}

		MulticastHitEffects(Hit);
	}

	Destroy();
}

void AProjectileActor::MulticastHitEffects_Implementation(FHitResult HitResult)
{
	uint8 SwitchExpression = 0;
	if (HitResult.PhysMaterial.IsValid())
	{
		SwitchExpression = StaticEnum<EPhysicalSurface>()->GetIndexByValue(UGameplayStatics::GetSurfaceType(HitResult));
	}

	switch (SwitchExpression)
	{
	case 0:
		// Head flesh
		break;
	case 1:
		// Body flesh
		break;
	case 2:
		// Arm flesh
		break;
	case 3:
		// Leg flesh
		break;
	case 4:
		// Stone
		break;
	case 5:
		// Metal
		break;
	case 6:
		// Wood
		break;
	default:
		return;;
	}
}