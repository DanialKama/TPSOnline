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
	
	UFUNCTION(Server, Reliable, WithValidation)
	void ServerStartStaminaDrain(EMovementState MovementState);
	
	UFUNCTION(Server, Reliable, WithValidation)
	void ServerStopStaminaDrain();
	
	/** Decreased stamina when jumping */
	UFUNCTION(Server, Reliable, WithValidation)
	void ServerJumpDrainStamina();

private:
	bool ServerStartStaminaDrain_Validate(EMovementState MovementState);
	void ServerStartStaminaDrain_Implementation(EMovementState MovementState);
	
	bool ServerStopStaminaDrain_Validate();
	void ServerStopStaminaDrain_Implementation();

	bool ServerJumpDrainStamina_Validate();
	void ServerJumpDrainStamina_Implementation();

	/** Decreased stamina when running */
	UFUNCTION(Server, Reliable, WithValidation)
	void ServerRunningDrainStamina();
	bool ServerRunningDrainStamina_Validate();
	void ServerRunningDrainStamina_Implementation();
	
	/** Decreased stamina when sprinting */
	UFUNCTION(Server, Reliable, WithValidation)
	void ServerSprintingDrainStamina();
	bool ServerSprintingDrainStamina_Validate();
	void ServerSprintingDrainStamina_Implementation();

	/** Restore stamina when the character stops or walks */
	UFUNCTION(Server, Reliable, WithValidation)
	void ServerRestoreStamina();
	bool ServerRestoreStamina_Validate();
	void ServerRestoreStamina_Implementation();

	/** Update stamina level on player UI */
	UFUNCTION(Client, Reliable)
	void ClientUpdateStamina();

public:
	UPROPERTY(Replicated)
	float CurrentStamina;
	
private:
	UPROPERTY(Replicated, EditAnywhere, Category = "Defaults", meta = (AllowPrivateAccess = true))
	float MaxStamina;
	
	UPROPERTY(Replicated, EditAnywhere, Category = "Defaults", meta = (AllowPrivateAccess = true))
	float RunningDrainAmount;

	UPROPERTY(Replicated, EditAnywhere, Category = "Defaults", meta = (AllowPrivateAccess = true))
	float SprintingDrainAmount;

	UPROPERTY(Replicated, EditAnywhere, Category = "Defaults", meta = (AllowPrivateAccess = true))
	float JumpingDrainAmount;
	
	UPROPERTY(Replicated, EditAnywhere, Category = "Defaults", meta = (AllowPrivateAccess = true))
	float RestoreStaminaAmount;
	
	UPROPERTY(Replicated, EditAnywhere, Category = "Defaults", meta = (AllowPrivateAccess = true))
	float RestoreStaminaDelay;
	
	FTimerHandle DrainStaminaTimer, RestoreStaminaTimer;
};