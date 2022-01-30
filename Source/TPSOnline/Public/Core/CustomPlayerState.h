// All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/PlayerState.h"
#include "CustomPlayerState.generated.h"

class AWeaponPickupActor;

UCLASS()
class TPSONLINE_API ACustomPlayerState : public APlayerState
{
	GENERATED_BODY()

// Functions
public:
	ACustomPlayerState();

	/** When the player left the session */
	UFUNCTION(Server, Reliable)
	void ServerPlayerDied();
	void ServerPlayerDied_Implementation();
	
// Variables
public:
	UPROPERTY(Replicated)
	AWeaponPickupActor* PrimaryWeapon;

	UPROPERTY(Replicated)
	AWeaponPickupActor* SecondaryWeapon;

	UPROPERTY(Replicated)
	AWeaponPickupActor* SidearmWeapon;

	/** 5.56 mm */
	UPROPERTY(Replicated)
	int32 FiveFiveSixAmmo;

	/** 7.62 mm */
	UPROPERTY(Replicated)
	int32 SevenSixTwoAmmo;

	/** .45 ACP */
	UPROPERTY(Replicated)
	int32 FortyFiveAmmo;

	/** 40 mm HE Grenade */
	UPROPERTY(Replicated)
	int32 HighExplosive;
};