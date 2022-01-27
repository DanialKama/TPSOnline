// All Rights Reserved.

#include "Actors/WeaponPickupActor.h"
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
	RotationIntensity = FRotator(0.0f, 0.0f, -5.0f);
	ControlTime = 0.25f;
	CrosshairRecoil = 10.0f;
	ControllerPitch = -0.5f;
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