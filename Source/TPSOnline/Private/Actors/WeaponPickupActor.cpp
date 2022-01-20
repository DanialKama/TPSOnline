// All Rights Reserved.

#include "Actors/WeaponPickupActor.h"
#include "Net/UnrealNetwork.h"

AWeaponPickupActor::AWeaponPickupActor()
{
	PrimaryActorTick.bCanEverTick = false;
}

// void AWeaponPickupActor::GetLifetimeReplicatedProps(TArray<FLifetimeProperty> &OutLifetimeProps) const
// {
// 	Super::GetLifetimeReplicatedProps(OutLifetimeProps);
//
// 	// Replicate to everyone
// 	DOREPLIFETIME(AWeaponPickupActor, IncreaseAmount);
// }

void AWeaponPickupActor::ServerUpdatePickupState_Implementation(APickupActor* Self, EPickupState NewState)
{
	if (GetLocalRole() == ROLE_Authority)
	{
		switch (NewState)
		{
		case 0:
			// Picked Up
			SetLifeSpan(0.0f);
			break;
		case 1:
			// Dropped
			SetLifeSpan(10.0f);
			break;
		case 2:
			// Used
			Self->Destroy();
			break;
		}
	}
}