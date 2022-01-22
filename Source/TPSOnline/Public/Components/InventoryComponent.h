// All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "Actors/WeaponPickupActor.h"
#include "Components/BaseComponent.h"
#include "InventoryComponent.generated.h"

UENUM()
enum class EWeaponToDo : uint8
{
	NoWeapon,
	Primary,
	Secondary,
	Sidearm
};

UCLASS()
class TPSONLINE_API UInventoryComponent : public UBaseComponent
{
	GENERATED_BODY()

// Functions
public:
	/** Sets default values for this component's properties */
	UInventoryComponent();

// Variables
public:
	UPROPERTY(Replicated)
	AWeaponPickupActor* PrimaryWeapon;

	UPROPERTY(Replicated)
	AWeaponPickupActor* SecondaryWeapon;

	UPROPERTY(Replicated)
	AWeaponPickupActor* SidearmWeapon;

	/** The weapon that is currently in the player's hand */
	UPROPERTY(Replicated)
	AWeaponPickupActor* CurrentWeapon;

	UPROPERTY(Replicated)
	EWeaponToDo CurrentWeaponSlot;
};