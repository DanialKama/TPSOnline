// All Rights Reserved.

#pragma once

#include "PickupEnums.generated.h"

UENUM()
enum class EPickupType : uint8
{
	Weapon,
	Ammo,
	Health
};

UENUM()
enum class EPickupState : uint8
{
	PickedUp,
	Dropped,
	Used
};

UENUM(BlueprintType)
enum class EWeaponType : uint8
{
	Pistol,
	SMG,
	Rifle,
	LMG,
	Shotgun,
	Sniper,
	Launcher
};

UENUM()
enum class EAmmoType : uint8
{
	FiveFiveSix	UMETA(DisplayName = "5.56"),
	SevenSixTwo	UMETA(DisplayName = "7.62"),
	FortyFive	UMETA(DisplayName = ".45")
};