// All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "BaseCharacter.h"
#include "Components/HealthComponent.h"
#include "Components/StaminaComponent.h"
#include "PlayerCharacter.generated.h"

UCLASS()
class TPSONLINE_API APlayerCharacter : public ABaseCharacter
{
	GENERATED_BODY()
	
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components", meta = (AllowPrivateAccess = "true"))
	class USpringArmComponent* SpringArm;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components", meta = (AllowPrivateAccess = "true"))
	class UCameraComponent* Camera;

// Functions
public:
	APlayerCharacter();
	
	/** For test */
	UFUNCTION(BlueprintCallable, Category = "Test")
	FORCEINLINE float GetStaminaLevel() const { return GetStaminaComponent()->CurrentStamina; }

	/** For test */
	UFUNCTION(BlueprintCallable, Category = "Test")
	FORCEINLINE float GetHealthLevel() const { return GetHealthComponent()->CurrentHealth; }
	
protected:
	/** Called to bind functionality to input */
	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;

	virtual void BeginPlay() override;

private:
	/** Called for forwards/backward input */
	void MoveForward(float Value);

	/** Called for side to side input */
	void MoveRight(float Value);

	/** 
	 * Called via input to turn at a given rate. 
	 * @param Rate	This is a normalized rate, i.e. 1.0 means 100% of desired turn rate
	 */
	void TurnAtRate(float Rate);

	/**
	 * Called via input to turn look up/down at a given rate. 
	 * @param Rate	This is a normalized rate, i.e. 1.0 means 100% of desired turn rate
	 */
	void LookUpAtRate(float Rate);

	void StartSprint();
	void StopSprint();

	void AttemptJump();

	void Interact();

	virtual void ClientUpdateHealth_Implementation(float NewHealth) override;
	virtual void ClientUpdateStamina_Implementation(float NewStamina) override;
	
// Variables
private:
	/** Base turn rate, in deg/sec. Other scaling may affect final turn rate. */
	UPROPERTY(EditDefaultsOnly, Category = "Defaults", meta = (AllowPrivateAccess = true))
	float BaseTurnRate;

	/** Base look up/down rate, in deg/sec. Other scaling may affect final rate. */
	UPROPERTY(EditDefaultsOnly, Category = "Defaults", meta = (AllowPrivateAccess = true))
	float BaseLookUpRate;

	UPROPERTY()
	APlayerController* PlayerController;

	UPROPERTY()
	class APlayerHUD* PlayerHUD;
};