// All Rights Reserved.

#include "Actors/PickupActor.h"

APickupActor::APickupActor()
{
	PrimaryActorTick.bCanEverTick = false;
}

void APickupActor::BeginPlay()
{
	Super::BeginPlay();

	if (GetLocalRole() == ROLE_Authority)
	{
		SetReplicates(true);
	}
}

bool APickupActor::ServerUpdatePickupState_Validate(APickupActor* Self, EPickupState NewState)
{
	if (Self)
	{
		return true;
	}
	return false;
}

void APickupActor::ServerUpdatePickupState_Implementation(APickupActor* Self, EPickupState NewState)
{
}