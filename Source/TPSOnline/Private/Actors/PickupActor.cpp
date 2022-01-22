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

void APickupActor::ServerUpdatePickupState_Implementation(EPickupState NewState)
{
}