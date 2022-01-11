// All Rights Reserved.

#include "Actors/PickupHealthActor.h"

APickupHealthActor::APickupHealthActor()
{
	PrimaryActorTick.bCanEverTick = false;

	MeshComponent = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("Mesh"));
	SetRootComponent(MeshComponent);
}

void APickupHealthActor::UpdatePickupState(EPickupState NewState)
{
	switch (NewState)
	{
	case 0:
		// Picked Up
		break;
	case 1:
		// Dropped
		break;
	case 2:
		// Used
		Destroy();
		break;
	}	
}