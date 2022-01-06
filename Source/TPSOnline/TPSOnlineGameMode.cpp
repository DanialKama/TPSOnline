// Copyright Epic Games, Inc. All Rights Reserved.

#include "TPSOnlineGameMode.h"
#include "TPSOnlineCharacter.h"
#include "UObject/ConstructorHelpers.h"

ATPSOnlineGameMode::ATPSOnlineGameMode()
{
	// set default pawn class to our Blueprinted character
	static ConstructorHelpers::FClassFinder<APawn> PlayerPawnBPClass(TEXT("/Game/ThirdPersonCPP/Blueprints/ThirdPersonCharacter"));
	if (PlayerPawnBPClass.Class != NULL)
	{
		DefaultPawnClass = PlayerPawnBPClass.Class;
	}
}
