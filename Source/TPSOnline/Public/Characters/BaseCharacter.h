// All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "BaseCharacter.generated.h"

enum class EMovementState : uint8
{
	Walk,
	Run,
	Sprint,
	Crouch,
	Prone
};

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

	virtual void SetStaminaLevel(float CurrentStamina);

protected:
	virtual void BeginPlay() override;
	virtual void Tick(float DeltaSeconds) override;

	FORCEINLINE UStaminaComponent* GetStaminaComponent() const { return StaminaComponent; }
	FORCEINLINE UHealthComponent* GetHealthComponent() const { return HealthComponent; }

protected:
	EMovementState MovementState;
	
	/** To check only once if character is moving or not */
	uint8 bDoOnceMoving : 1, bDoOnceStopped : 1;
};