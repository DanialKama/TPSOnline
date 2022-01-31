// Copyright 2022 Danial Kamali. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/HUD.h"
#include "PlayerHUD.generated.h"

UCLASS()
class TPSONLINE_API APlayerHUD : public AHUD
{
	GENERATED_BODY()

// Functions
public:
	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "PlayerHUD")
	void Initialize();

	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "PlayerHUD")
	void SetCrosshairVisibility(ESlateVisibility Visibility);
	
	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "PlayerHUD")
	void AddRecoil(float Recoil, float ControlTime);
	
	/** Update health level on player UI */
	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "PlayerHUD")
	void UpdateHealth(float NewHealth);

	/** Update stamina level on player UI */
	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "PlayerHUD")
	void UpdateStamina(float NewStamina);
};