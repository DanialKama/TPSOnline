// All Rights Reserved.

#include "Actors/AmmoPickupActor.h"

AAmmoPickupActor::AAmmoPickupActor()
{
	PrimaryActorTick.bCanEverTick = false;

	MeshComponent = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("Mesh"));
	SetRootComponent(MeshComponent);
	MeshComponent->SetComponentTickEnabled(false);
	MeshComponent->CanCharacterStepUpOn = ECB_No;
	MeshComponent->SetGenerateOverlapEvents(false);
	MeshComponent->SetCollisionProfileName("Pickup");
	
	// Initialize variables
	PickupType = EPickupType::Ammo;
	AmmoType = EAmmoType::FiveFiveSix;
	Amount = 90;
}

void AAmmoPickupActor::OnRep_PickupState()
{
	switch (PickupState)
	{
	case 0: case 2:
		// Picked up, Used
		if (Amount <= 0)
		{
			Destroy();
		}
		break;
	case 1:
		// Dropped
		MeshComponent->SetSimulatePhysics(true);
		MeshComponent->SetCollisionProfileName("Pickup");
		break;
	}
}