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
};