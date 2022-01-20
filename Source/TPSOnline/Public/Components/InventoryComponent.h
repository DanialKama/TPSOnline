// All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "Actors/WeaponPickupActor.h"
#include "Components/BaseComponent.h"
#include "InventoryComponent.generated.h"

UCLASS()
class TPSONLINE_API UInventoryComponent : public UBaseComponent
{
	GENERATED_BODY()

// Functions
public:
	/** Sets default values for this component's properties */
	UInventoryComponent();

// Variables
private:
	UPROPERTY(Replicated)
	AWeaponPickupActor* PrimaryWeapon;

	UPROPERTY(Replicated)
	AWeaponPickupActor* SecondaryWeapon;

	UPROPERTY(Replicated)
	AWeaponPickupActor* SidearmWeapon;
};