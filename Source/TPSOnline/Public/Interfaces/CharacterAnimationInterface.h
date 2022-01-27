// All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "UObject/Interface.h"
#include "CharacterAnimationInterface.generated.h"

// This class does not need to be modified.
UINTERFACE(MinimalAPI)
class UCharacterAnimationInterface : public UInterface
{
	GENERATED_BODY()
};

class TPSONLINE_API ICharacterAnimationInterface
{
	GENERATED_BODY()

	// Add interface functions to this class. This is the class that will be inherited to implement this interface.
public:
	/** Add recoil to character's spine
	* @param RotationIntensity	To rotate the character's spine
	* @param ControlTime		Use as alpha in lerp
	*/
	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "CharacterAnimationInterface")
	void AddRecoil(FRotator RotationIntensity, float ControlTime);
};
