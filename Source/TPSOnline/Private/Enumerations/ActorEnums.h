// Copyright 2022 Danial Kamali. All Rights Reserved.

#pragma once

#include "ActorEnums.generated.h"

// TODO: Don't use UENUM() macro if these enums are not used by blueprints

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

/** To use in player UI */
UENUM(BlueprintType)
enum class EWeaponName : uint8
{
	Default			UMETA(DisplayName = "Default"),
	AssaultRifle	UMETA(DisplayName = "Assault Rifle"),
	Pistol			UMETA(DisplayName = "Pistol")
};