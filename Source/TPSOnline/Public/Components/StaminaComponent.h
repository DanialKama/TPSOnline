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

// Functions
public:	
	/** Sets default values for this component's properties */
	UStaminaComponent();
	
	UFUNCTION(Server, Reliable)
	void ServerStartStaminaDrain(EMovementState MovementState);
	
	UFUNCTION(Server, Reliable)
	void ServerStopStaminaDrain();
	
	/** Decreased stamina when jumping */
	UFUNCTION(Server, Reliable)
	void ServerJumpDrainStamina();

private:
	virtual void ServerInitialize_Implementation(UBaseComponent* Self) override;

	void ServerStartStaminaDrain_Implementation(EMovementState MovementState);
	
	void ServerStopStaminaDrain_Implementation();

	void ServerJumpDrainStamina_Implementation();
	
	/** Decreased stamina when running */
	UFUNCTION(Server, Reliable)
	void ServerRunningDrainStamina();
	void ServerRunningDrainStamina_Implementation();
	
	/** Decreased stamina when sprinting */
	UFUNCTION(Server, Reliable)
	void ServerSprintingDrainStamina();
	void ServerSprintingDrainStamina_Implementation();

	/** Restore stamina when the character stops or walks */
	UFUNCTION(Server, Reliable)
	void ServerRestoreStamina();
	void ServerRestoreStamina_Implementation();

// Variables
public:
	UPROPERTY(Replicated, EditAnywhere, Category = "Defaults", meta = (ClampMin = "0.0", UIMin = "0.0", AllowPrivateAccess = true))
	float MaxStamina;
	
	UPROPERTY(Replicated)
	float CurrentStamina;
	
private:
	UPROPERTY(Replicated, EditAnywhere, Category = "Defaults", meta = (ClampMin = "0.0", UIMin = "0.0", AllowPrivateAccess = true))
	float RunningDrainAmount;

	UPROPERTY(Replicated, EditAnywhere, Category = "Defaults", meta = (ClampMin = "0.0", UIMin = "0.0", AllowPrivateAccess = true))
	float SprintingDrainAmount;

	UPROPERTY(Replicated, EditAnywhere, Category = "Defaults", meta = (ClampMin = "0.0", UIMin = "0.0", AllowPrivateAccess = true))
	float JumpingDrainAmount;
	
	UPROPERTY(Replicated, EditAnywhere, Category = "Defaults", meta = (ClampMin = "0.0", UIMin = "0.0", AllowPrivateAccess = true))
	float RestoreStaminaAmount;
	
	UPROPERTY(Replicated, EditAnywhere, Category = "Defaults", meta = (ClampMin = "0.0", UIMin = "0.0", AllowPrivateAccess = true))
	float RestoreStaminaDelay;
	
	FTimerHandle DrainStaminaTimer, RestoreStaminaTimer;
};