// All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "Components/BaseComponent.h"
#include "Characters/BaseCharacter.h"
#include "StaminaComponent.generated.h"

UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class TPSONLINE_API UStaminaComponent : public UBaseComponent
{
	GENERATED_BODY()

public:	
	/** Sets default values for this component's properties */
	UStaminaComponent();
	virtual void BeginPlay() override;
	virtual void Initialize() override;

	void StartStaminaDrain(EMovementState MovementState);
	void StopStaminaDrain();

private:
	void RunningDrainStamina();
	
	UFUNCTION(Server, Reliable, WithValidation)
	void ServerRunningDrainStamina();
	bool ServerRunningDrainStamina_Validate();
	void ServerRunningDrainStamina_Implementation();

	void SprintingDrainStamina();
	
	UFUNCTION(Server, Reliable, WithValidation)
	void ServerSprintingDrainStamina();
	bool ServerSprintingDrainStamina_Validate();
	void ServerSprintingDrainStamina_Implementation();

	void RestoreStamina();

	UFUNCTION(Server, Reliable, WithValidation)
	void ServerRestoreStamina();
	bool ServerRestoreStamina_Validate();
	void ServerRestoreStamina_Implementation();

public:
	UPROPERTY(EditAnywhere, Category = "Defaults", meta = (AllowPrivateAccess = true))
	float MaxStamina;
	
	UPROPERTY(Replicated)
	float CurrentStamina;
	
private:
	UPROPERTY(EditAnywhere, Category = "Defaults", meta = (AllowPrivateAccess = true))
	float RunningDrainAmount;

	UPROPERTY(EditAnywhere, Category = "Defaults", meta = (AllowPrivateAccess = true))
	float SprintingDrainAmount;

	UPROPERTY(EditAnywhere, Category = "Defaults", meta = (AllowPrivateAccess = true))
	float RestoreStaminaAmount;
	
	UPROPERTY(EditAnywhere, Category = "Defaults", meta = (AllowPrivateAccess = true))
	float RestoreStaminaDelay;
	
	FTimerHandle DrainStaminaTimer, RestoreStaminaTimer;
};