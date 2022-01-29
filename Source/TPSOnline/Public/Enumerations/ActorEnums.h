// All Rights Reserved.

#pragma once

#include "ActorEnums.generated.h"

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

UENUM(BlueprintType)
enum class EAmmoType : uint8
{
	FiveFiveSix		UMETA(DisplayName = "5.56"),	// 5.56 mm
	SevenSixTwo		UMETA(DisplayName = "7.62"),	// 7.62 mm
	FortyFive		UMETA(DisplayName = ".45"),		// .45 ACP
	HighExplosive	UMETA(DisplayName = "40 mm HE")	// 40 mm HE Grenade
};