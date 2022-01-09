// All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "Enums/CharacterEnums.h"
#include "BaseCharacter.generated.h"

UCLASS()
class TPSONLINE_API ABaseCharacter : public ACharacter
{
	GENERATED_BODY()

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components", meta = (AllowPrivateAccess = "true"))
	class UHealthComponent* HealthComponent;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components", meta = (AllowPrivateAccess = "true"))
	class UStaminaComponent* StaminaComponent;

public:
	ABaseCharacter();

protected:
	virtual void Tick(float DeltaSeconds) override;

protected:
	EMovementState MovementState;

private:
	/** To check only once if character is moving or not */
	uint8 bDoOnceMoving : 1, bDoOnceStopped : 1;
};