// All Rights Reserved.

#include "Actors/WeaponPickupActor.h"
#include "Kismet/GameplayStatics.h"
#include "Net/UnrealNetwork.h"

AWeaponPickupActor::AWeaponPickupActor()
{
	PrimaryActorTick.bCanEverTick = false;

	SkeletalMesh = CreateDefaultSubobject<USkeletalMeshComponent>(TEXT("Skeletal Mesh"));
	SetRootComponent(SkeletalMesh);
	SkeletalMesh->SetComponentTickEnabled(false);
	SkeletalMesh->bApplyImpulseOnDamage = false;
	SkeletalMesh->CanCharacterStepUpOn = ECB_No;
	SkeletalMesh->SetCollisionProfileName("Pickup");

	// Initialize variables
	WeaponType = EWeaponType::Pistol;
	AmmoType = EAmmoType::FortyFive;
	bIsAutomatic = false;
	TimeBetweenShots = 0.2;
}

void AWeaponPickupActor::ServerSpawnProjectile_Implementation()
{
	// TODO - Spawn projectile
	MulticastWeaponEffects();
}

void AWeaponPickupActor::MulticastWeaponEffects_Implementation()
{
	UGameplayStatics::SpawnEmitterAtLocation(GetWorld(), Effects.MuzzleFlash, SkeletalMesh->GetSocketLocation(FName("MuzzleSocket")), SkeletalMesh->GetSocketRotation(FName("MuzzleSocket")), Effects.MuzzleFlashScale);
}

void AWeaponPickupActor::OnRep_PickupState()
{
	switch (PickupState)
	{
	case 0:
		// Picked up
		SkeletalMesh->SetSimulatePhysics(false);
		SkeletalMesh->SetCollisionProfileName("Weapon");
		SetLifeSpan(0.0f);
		break;
	case 1:
		// Dropped
		SetOwner(nullptr);
		SkeletalMesh->SetSimulatePhysics(true);
		SkeletalMesh->SetCollisionProfileName("Pickup");
		SetLifeSpan(FMath::FRandRange(10.0f, 15.0f));
		break;
	case 2:
		// Used
		Destroy();
		break;
	}
}