// All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/GameModeBase.h"
#include "DeathmatchGameMode.generated.h"

UCLASS()
class TPSONLINE_API ADeathmatchGameMode : public AGameModeBase
{
	GENERATED_BODY()

// Functions
public:
	/** Respawn Player */
	UFUNCTION(Server, Reliable)
	void ServerStartRespawn(AController* Controller);

protected:
	virtual void Logout(AController* Exiting) override;

private:
	void ServerStartRespawn_Implementation(AController* Controller);

	UFUNCTION(Server, Reliable, WithValidation)
	void ServerRespawn();
	bool ServerRespawn_Validate();
	void ServerRespawn_Implementation();

// Variables
private:
	UPROPERTY()
	TArray<AController*> ControllersToRespawn;
	
	UPROPERTY()
	TArray<AController*> ExitedControllers;
};