// All Rights Reserved.

#include "Core/CustomPlayerState.h"
#include "Actors/WeaponPickupActor.h"
#include "Net/UnrealNetwork.h"

ACustomPlayerState::ACustomPlayerState()
{
	// Initialize variables
	PrimaryWeapon = SecondaryWeapon = SidearmWeapon = nullptr;
	SevenSixTwoAmmo = FiveFiveSixAmmo = 60;
	FortyFiveAmmo = 36;
}

void ACustomPlayerState::GetLifetimeReplicatedProps(TArray<FLifetimeProperty> &OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	// Replicate to everyone
	DOREPLIFETIME(ACustomPlayerState, PrimaryWeapon);
	DOREPLIFETIME(ACustomPlayerState, SecondaryWeapon);
	DOREPLIFETIME(ACustomPlayerState, SidearmWeapon);
	DOREPLIFETIME(ACustomPlayerState, FiveFiveSixAmmo);
	DOREPLIFETIME(ACustomPlayerState, SevenSixTwoAmmo);
	DOREPLIFETIME(ACustomPlayerState, FortyFiveAmmo);
}

void ACustomPlayerState::ServerPlayerDied_Implementation()
{
	const FDetachmentTransformRules DetachmentRules = FDetachmentTransformRules(EDetachmentRule::KeepWorld, EDetachmentRule::KeepWorld, EDetachmentRule::KeepRelative, false);
	if (PrimaryWeapon)
	{
		PrimaryWeapon->DetachFromActor(DetachmentRules);
		PrimaryWeapon->PickupState = EPickupState::Dropped;
		PrimaryWeapon->OnRep_PickupState();
		PrimaryWeapon = nullptr;
	}

	if (SecondaryWeapon)
	{
		SecondaryWeapon->DetachFromActor(DetachmentRules);
		SecondaryWeapon->PickupState = EPickupState::Dropped;
		SecondaryWeapon->OnRep_PickupState();
		SecondaryWeapon = nullptr;
	}

	if (SidearmWeapon)
	{
		SidearmWeapon->DetachFromActor(DetachmentRules);
		SidearmWeapon->PickupState = EPickupState::Dropped;
		SidearmWeapon->OnRep_PickupState();
		SidearmWeapon = nullptr;
	}
}