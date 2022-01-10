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
	
	virtual void Initialize() override;
	
	void StartStaminaDrain(EMovementState MovementState);
	void StopStaminaDrain();
	
	/** Decreased stamina when jumping */
	void JumpDrainStamina();

private:
	UFUNCTION(Server, Reliable, WithValidation)
	void ServerStartStaminaDrain(EMovementState MovementState);
	bool ServerStartStaminaDrain_Validate(EMovementState MovementState);
	void ServerStartStaminaDrain_Implementation(EMovementState MovementState);
	
	UFUNCTION(Server, Reliable, WithValidation)
	void ServerStopStaminaDrain();
	bool ServerStopStaminaDrain_Validate();
	void ServerStopStaminaDrain_Implementation();
	
	UFUNCTION(Server, Reliable, WithValidation)
	void ServerJumpDrainStamina();
	bool ServerJumpDrainStamina_Validate();
	void ServerJumpDrainStamina_Implementation();
	
	UFUNCTION(Server, Reliable, WithValidation)
	void ServerRunningDrainStamina();
	bool ServerRunningDrainStamina_Validate();
	/** Decreased stamina when running */
	UFUNCTION()
	void ServerRunningDrainStamina_Implementation();
	
	UFUNCTION(Server, Reliable, WithValidation)
	void ServerSprintingDrainStamina();
	bool ServerSprintingDrainStamina_Validate();
	/** Decreased stamina when sprinting */
	UFUNCTION()
	void ServerSprintingDrainStamina_Implementation();
	
	UFUNCTION(Server, Reliable, WithValidation)
	void ServerRestoreStamina();
	bool ServerRestoreStamina_Validate();
	/** Restore stamina when the character stops or walks */
	UFUNCTION()
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
	float JumpingDrainAmount;
	
	UPROPERTY(EditAnywhere, Category = "Defaults", meta = (AllowPrivateAccess = true))
	float RestoreStaminaAmount;
	
	UPROPERTY(EditAnywhere, Category = "Defaults", meta = (AllowPrivateAccess = true))
	float RestoreStaminaDelay;
	
	FTimerHandle DrainStaminaTimer, RestoreStaminaTimer;
};