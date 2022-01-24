// All Rights Reserved.

#include "Core/CustomPlayerState.h"
#include "Actors/WeaponPickupActor.h"
#include "Net/UnrealNetwork.h"

ACustomPlayerState::ACustomPlayerState()
{
	// Initialize variables
	PrimaryWeapon = SecondaryWeapon = SidearmWeapon = nullptr;
}

void ACustomPlayerState::GetLifetimeReplicatedProps(TArray<FLifetimeProperty> &OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	// Replicate to everyone
	DOREPLIFETIME(ACustomPlayerState, PrimaryWeapon);
	DOREPLIFETIME(ACustomPlayerState, SecondaryWeapon);
	DOREPLIFETIME(ACustomPlayerState, SidearmWeapon);
}

void ACustomPlayerState::ServerPlayerDied_Implementation()
{
	const FDetachmentTransformRules DetachmentRules = FDetachmentTransformRules(EDetachmentRule::KeepWorld, EDetachmentRule::KeepWorld, EDetachmentRule::KeepRelative, false);
	if (PrimaryWeapon)
	{
		PrimaryWeapon->DetachFromActor(DetachmentRules);
		PrimaryWeapon->PickupState = EPickupState::Dropped;
		PrimaryWeapon->OnRep_UpdatePickupState();
		PrimaryWeapon = nullptr;
	}

	if (SecondaryWeapon)
	{
		SecondaryWeapon->DetachFromActor(DetachmentRules);
		SecondaryWeapon->PickupState = EPickupState::Dropped;
		SecondaryWeapon->OnRep_UpdatePickupState();
		SecondaryWeapon = nullptr;
	}

	if (SidearmWeapon)
	{
		SidearmWeapon->DetachFromActor(DetachmentRules);
		SidearmWeapon->PickupState = EPickupState::Dropped;
		SidearmWeapon->OnRep_UpdatePickupState();
		SidearmWeapon = nullptr;
	}
}