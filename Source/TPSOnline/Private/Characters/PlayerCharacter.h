// Copyright 2022 Danial Kamali. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "BaseCharacter.h"
#include "Components/TimelineComponent.h"
#include "PlayerCharacter.generated.h"

UCLASS()
class APlayerCharacter : public ABaseCharacter
{
	GENERATED_BODY()
	
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components", meta = (AllowPrivateAccess = "true"))
	TObjectPtr<class USpringArmComponent> SpringArm;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components", meta = (AllowPrivateAccess = "true"))
	TObjectPtr<UCameraComponent> Camera;

	UPROPERTY()
	TObjectPtr<UTimelineComponent> AimTimeline;

// Functions
public:
	APlayerCharacter();
	
protected:
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

	void StartAim();
	void StopAim();

	UFUNCTION()
	void AimTimeLineUpdate(float Value);
	
	UFUNCTION()
	void AimTimeLineFinished();

	void StartFireWeapon();
	void StopFireWeapon();

	/** Add recoil to character's spine and player crosshair */
	void AddRecoil();

	/** Reload the current weapon */
	void ReloadWeapon();
	
	/** Attempt to drop the current weapon */
	void DropCurrentWeapon();

	virtual void OnRep_CurrentWeapon() override;

	/** Find remained ammo in inventory based on the current weapon's ammo type. */
	int32 FindCurrentAmmo() const;

	virtual void ClientUpdateHealth_Implementation(float NewHealth) override;
	virtual void ClientUpdateStamina_Implementation(float NewStamina) override;

	void UpdateControllerPitch(float Value);

	/** Update Look Up Pitch on server and replicate it */
	UFUNCTION(Server, Reliable)
	void ServerUpdateLookUp(float Pitch);
	void ServerUpdateLookUp_Implementation(float Pitch);

// Variables
private:
	UPROPERTY(EditDefaultsOnly, Category = "Defaults", meta = (AllowPrivateAccess = "true"))
	TObjectPtr<UCurveFloat> AimFloatCurve;
	
	/** Base turn rate, in deg/sec. Other scaling may affect final turn rate. */
	UPROPERTY(EditDefaultsOnly, Category = "Defaults", meta = (AllowPrivateAccess = true))
	float BaseTurnRate;

	/** Base look up/down rate, in deg/sec. Other scaling may affect final rate. */
	UPROPERTY(EditDefaultsOnly, Category = "Defaults", meta = (AllowPrivateAccess = true))
	float BaseLookUpRate;

	UPROPERTY(Replicated)
	TObjectPtr<APlayerController> PlayerControllerRef;

	UPROPERTY()
	TObjectPtr<class APlayerHUD> PlayerHUD;

	/** Using for aim offset */
	UPROPERTY(Replicated, BlueprintReadOnly, Category = "Defaults", meta = (AllowPrivateAccess = true))
	float LookUpPitch;

	uint8 bDoOnceCrouch : 1, bCharacterAnimationInterface : 1;

	UPROPERTY()
	TObjectPtr<UAnimInstance> AnimInstance;
	
	/** Enum data indicating the direction the Timeline is playing */
	TEnumAsByte<ETimelineDirection::Type> TimeLineDirection;

	FTimerHandle RecoilTimer;
};
