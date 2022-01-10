// All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "UObject/Interface.h"
#include "WidgetInterface.generated.h"

// This class does not need to be modified.
UINTERFACE(MinimalAPI)
class UWidgetInterface : public UInterface
{
	GENERATED_BODY()
};

class TPSONLINE_API IWidgetInterface
{
	GENERATED_BODY()

	// Add interface functions to this class. This is the class that will be inherited to implement this interface.
public:
	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "WidgetInterface")
	void UpdateStaminaLevel(float CurrentStamina);
};
