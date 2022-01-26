// All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "BaseCharacter.h"
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
	
protected:
	/** Called to bind functionality to input */
	virtual void SetupPlayerInputComponent(UInputComponent* PlayerInputComponent) override;
	
	virtual void BeginPlay() override;
	virtual void PossessedBy(AController* NewController) override;
	virtual void Destroyed() override;

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

	void ToggleCrouch();

	/** Stop player from spamming the crouch */
	void ResetCrouch();

	void Interact();

	/** Attempt to drop the current weapon */
	void DropCurrentWeapon();

	virtual void ClientUpdateHealth_Implementation(float NewHealth) override;
	virtual void ClientUpdateStamina_Implementation(float NewStamina) override;

	void UpdateControllerPitch(float Value);

	/** Update Look Up Pitch on server and replicate it */
	UFUNCTION(Server, Reliable)
	void ServerUpdateLookUp(float Pitch);
	void ServerUpdateLookUp_Implementation(float Pitch);

// Variables
private:
	/** Base turn rate, in deg/sec. Other scaling may affect final turn rate. */
	UPROPERTY(EditDefaultsOnly, Category = "Defaults", meta = (AllowPrivateAccess = true))
	float BaseTurnRate;

	/** Base look up/down rate, in deg/sec. Other scaling may affect final rate. */
	UPROPERTY(EditDefaultsOnly, Category = "Defaults", meta = (AllowPrivateAccess = true))
	float BaseLookUpRate;

	UPROPERTY(Replicated)
	APlayerController* PlayerControllerRef;

	UPROPERTY()
	class APlayerHUD* PlayerHUD;

	/** Using for aim offset */
	UPROPERTY(Replicated, VisibleAnywhere, BlueprintReadOnly, Category = "Defaults", meta = (AllowPrivateAccess = true))
	float LookUpPitch;

	uint8 bDoOnceCrouch : 1;
};