// All Rights Reserved.

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
	/** Update health level on player UI */
	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "PlayerHUD")
	void UpdateHealth(float NewHealth);

	/** Update stamina level on player UI */
	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "PlayerHUD")
	void UpdateStamina(float NewStamina);
};