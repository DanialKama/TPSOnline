// All Rights Reserved.

#include "Actors/PickupActor.h"
#include "Net/UnrealNetwork.h"

APickupActor::APickupActor()
{
	PrimaryActorTick.bCanEverTick = false;
}

void APickupActor::GetLifetimeReplicatedProps(TArray<FLifetimeProperty> &OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	// Replicate to everyone
	DOREPLIFETIME(APickupActor, PickupState);
}

void APickupActor::BeginPlay()
{
	Super::BeginPlay();

	if (GetLocalRole() == ROLE_Authority)
	{
		SetReplicates(true);
		SetReplicateMovement(true);
	}
}

void APickupActor::OnRep_UpdatePickupState()
{
	switch (PickupState)
	{
	case 0:
		// Picked up
		break;
	case 1:
		// Dropped
		break;
	case 2:
		// Used
		break;
	}
}