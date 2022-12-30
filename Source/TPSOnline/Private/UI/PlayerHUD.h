// Copyright 2022 Danial Kamali. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "Enumerations/ActorEnums.h"
#include "GameFramework/HUD.h"
#include "PlayerHUD.generated.h"

UCLASS()
class APlayerHUD : public AHUD
{
	GENERATED_BODY()

// Functions
public:
	UFUNCTION(BlueprintImplementableEvent, BlueprintCallable, Category = "PlayerHUD")
	void Initialize();

	UFUNCTION(BlueprintImplementableEvent, BlueprintCallable, Category = "PlayerHUD")
	void SetCrosshairVisibility(ESlateVisibility Visibility);
	
	UFUNCTION(BlueprintImplementableEvent, BlueprintCallable, Category = "PlayerHUD")
	void AddCrosshairRecoil(float Recoil, float ControlTime);
	
	/** Update health level on player UI */
	UFUNCTION(BlueprintImplementableEvent, BlueprintCallable, Category = "PlayerHUD")
	void UpdateHealth(float NewHealth);

	/** Update stamina level on player UI */
	UFUNCTION(BlueprintImplementableEvent, BlueprintCallable, Category = "PlayerHUD")
	void UpdateStamina(float NewStamina);

	UFUNCTION(BlueprintImplementableEvent, BlueprintCallable, Category = "PlayerHUD")
	void SetWeaponInfoVisibility(ESlateVisibility Visibility);
	
	UFUNCTION(BlueprintImplementableEvent, BlueprintCallable, Category = "PlayerHUD")
	void UpdateWeaponInfo(EWeaponName WeaponName, int32 CurrentAmmo);

	/** Updating Current ammo inside the weapon's magazine */
	UFUNCTION(BlueprintImplementableEvent, BlueprintCallable, Category = "PlayerHUD")
	void UpdateCurrentMagAmmo(int32 CurrentMagAmmo);
};
