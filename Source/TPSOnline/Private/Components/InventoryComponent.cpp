// All Rights Reserved.

#include "Components/InventoryComponent.h"
#include "Net/UnrealNetwork.h"

UInventoryComponent::UInventoryComponent()
{
	PrimaryComponentTick.bCanEverTick = false;

	// Initialize variables
	PrimaryWeapon = SecondaryWeapon = SidearmWeapon = CurrentWeapon = nullptr;
	CurrentWeaponSlot = EWeaponToDo::NoWeapon;
}

void UInventoryComponent::GetLifetimeReplicatedProps(TArray<FLifetimeProperty> &OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	// Replicate to everyone
	DOREPLIFETIME(UInventoryComponent, PrimaryWeapon);
	DOREPLIFETIME(UInventoryComponent, SecondaryWeapon);
	DOREPLIFETIME(UInventoryComponent, SidearmWeapon);
	DOREPLIFETIME(UInventoryComponent, CurrentWeapon);
	DOREPLIFETIME(UInventoryComponent, CurrentWeaponSlot);
}