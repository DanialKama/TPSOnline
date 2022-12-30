// Copyright 2022 Danial Kamali. All Rights Reserved.

#pragma once

#include "CharacterEnums.generated.h"

UENUM(BlueprintType)
enum class EMovementState : uint8
{
	Walk	UMETA(DisplayName = "Walking"),
	Run		UMETA(DisplayName = "Running"),
	Sprint	UMETA(DisplayName = "Sprinting"),
	Crouch	UMETA(DisplayName = "Crouch"),
	Prone	UMETA(DisplayName = "Prone")
};

UENUM()
enum class EWeaponToDo : uint8
{
	NoWeapon,
	Primary,
	Secondary,
	Sidearm
};