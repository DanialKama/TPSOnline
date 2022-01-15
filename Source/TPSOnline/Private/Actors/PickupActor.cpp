// All Rights Reserved.

#include "Actors/PickupActor.h"

APickupActor::APickupActor()
{
	PrimaryActorTick.bCanEverTick = false;
}

bool APickupActor::MulticastUpdatePickupState_Validate(APickupActor* Self, EPickupState NewState)
{
	if (Self)
	{
		return true;
	}
	return false;
}

void APickupActor::MulticastUpdatePickupState_Implementation(APickupActor* Self, EPickupState NewState)
{
}