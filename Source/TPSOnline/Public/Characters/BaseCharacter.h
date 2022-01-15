// All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "Actors/PickupActor.h"
#include "GameFramework/Character.h"
#include "BaseCharacter.generated.h"

UENUM()
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

// Functions
public:
	ABaseCharacter();

	virtual void SetHealthLevel(float CurrentHealth);
	virtual void SetStaminaLevel(float CurrentStamina);

protected:
	virtual void BeginPlay() override;
	virtual void Tick(float DeltaSeconds) override;
	virtual void OnMovementModeChanged(EMovementMode PrevMovementMode, uint8 PreviousCustomMode) override;

	/** Change Movement State to Sprint or Walk based on previous Movement State */
	UFUNCTION(Server, Reliable, WithValidation)
	void ServerChangeMovementState(EMovementState NewMovementState);

	UFUNCTION(Server, Reliable, WithValidation)
	void ServerInteract(ABaseCharacter* Self);
	
	FORCEINLINE UStaminaComponent* GetStaminaComponent() const { return StaminaComponent; }
	FORCEINLINE UHealthComponent* GetHealthComponent() const { return HealthComponent; }

private:
	bool ServerChangeMovementState_Validate(EMovementState NewMovementState);
	void ServerChangeMovementState_Implementation(EMovementState NewMovementState);

	bool ServerInteract_Validate(ABaseCharacter* Self);
	void ServerInteract_Implementation(ABaseCharacter* Self);

	APickupActor* FindPickup(ABaseCharacter* Self) const;

// Variables
protected:
	UPROPERTY(Replicated)
	EMovementState MovementState;

	UPROPERTY(Replicated)
	float MovementScale;
	
	/** To check only once if character is moving or not */
	uint8 bDoOnceMoving : 1, bDoOnceStopped : 1;
};