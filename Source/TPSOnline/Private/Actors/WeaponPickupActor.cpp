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
}

void AWeaponPickupActor::ServerUpdatePickupState_Implementation(APickupActor* Self, EPickupState NewState)
{
	if (GetLocalRole() == ROLE_Authority)
	{
		switch (NewState)
		{
		case 0:
			// Picked Up
			SkeletalMesh->SetCollisionProfileName("Weapon");
			SetLifeSpan(0.0f);
			break;
		case 1:
			// Dropped
			SkeletalMesh->SetCollisionProfileName("Pickup");
			SetLifeSpan(10.0f);
			break;
		case 2:
			// Used
			Self->Destroy();
			break;
		}
	}
}