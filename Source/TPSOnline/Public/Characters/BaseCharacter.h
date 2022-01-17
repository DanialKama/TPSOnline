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

	/** Use a single trace to find if the hit actor is a pickup */
	APickupActor* FindPickup(ABaseCharacter* Self) const;

	UFUNCTION(Server, Reliable)
	void ServerSetHealthLevel(float CurrentHealth);

	UFUNCTION(Server, Reliable)
	void ServerSetStaminaLevel(float CurrentStamina);

protected:
	virtual void BeginPlay() override;
	virtual void Tick(float DeltaSeconds) override;
	virtual void OnMovementModeChanged(EMovementMode PrevMovementMode, uint8 PreviousCustomMode) override;

	/** Change Movement State to Sprint or Walk based on previous Movement State */
	UFUNCTION(Server, Reliable)
	void ServerChangeMovementState(EMovementState NewMovementState);

	UFUNCTION(Server, Reliable, WithValidation)
	void ServerInteractWithWeapon(ABaseCharacter* Self);

	UFUNCTION(Server, Reliable, WithValidation)
	void ServerInteractWithAmmo(ABaseCharacter* Self);
	
	UFUNCTION(Server, Reliable, WithValidation)
	void ServerInteractWithHealth(ABaseCharacter* Self);
	
	virtual void ClientUpdateHealth_Implementation(float NewHealth);
	virtual void ClientUpdateStamina_Implementation(float NewStamina);

	FORCEINLINE UStaminaComponent* GetStaminaComponent() const { return StaminaComponent; }
	FORCEINLINE UHealthComponent* GetHealthComponent() const { return HealthComponent; }

private:
	void ServerChangeMovementState_Implementation(EMovementState NewMovementState);

	bool ServerInteractWithWeapon_Validate(ABaseCharacter* Self);
	void ServerInteractWithWeapon_Implementation(ABaseCharacter* Self);

	bool ServerInteractWithAmmo_Validate(ABaseCharacter* Self);
	void ServerInteractWithAmmo_Implementation(ABaseCharacter* Self);

	bool ServerInteractWithHealth_Validate(ABaseCharacter* Self);
	void ServerInteractWithHealth_Implementation(ABaseCharacter* Self);

	void ServerSetHealthLevel_Implementation(float CurrentHealth);
	void ServerSetStaminaLevel_Implementation(float CurrentStamina);
	
	/** Update health on player UI */
	UFUNCTION(Client, Unreliable)
	void ClientUpdateHealth(float NewHealth);
	
	/** Update stamina on player UI */
	UFUNCTION(Client, Unreliable)
	void ClientUpdateStamina(float NewStamina);

// Variables
protected:
	UPROPERTY(Replicated)
	EMovementState MovementState;

	UPROPERTY(Replicated)
	float MovementScale;
	
	/** To check only once if character is moving or not */
	uint8 bDoOnceMoving : 1, bDoOnceStopped : 1;
};