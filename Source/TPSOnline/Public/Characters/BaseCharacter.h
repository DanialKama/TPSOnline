// All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "Actors/PickupActor.h"
#include "Components/InventoryComponent.h"
#include "GameFramework/Character.h"
#include "BaseCharacter.generated.h"

class AWeaponPickupActor;

UENUM(BlueprintType)
enum class EMovementState : uint8
{
	Walk	UMETA(DisplayName = "Walking"),
	Run		UMETA(DisplayName = "Running"),
	Sprint	UMETA(DisplayName = "Sprinting"),
	Crouch	UMETA(DisplayName = "Crouch"),
	Prone	UMETA(DisplayName = "Prone")
};

UCLASS()
class TPSONLINE_API ABaseCharacter : public ACharacter
{
	GENERATED_BODY()

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components", meta = (AllowPrivateAccess = "true"))
	class UHealthComponent* HealthComponent;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components", meta = (AllowPrivateAccess = "true"))
	class UStaminaComponent* StaminaComponent;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components", meta = (AllowPrivateAccess = "true"))
	class UInventoryComponent* InventoryComponent;

// Functions
public:
	ABaseCharacter();

	/** Use a single trace to find if the hit actor is a pickup */
	APickupActor* FindPickup() const;

	UFUNCTION(Server, Reliable)
	void ServerSetHealthLevel(ABaseCharacter* ComponentOwner, float CurrentHealth, float MaxHealth);

	UFUNCTION(Server, Reliable)
	void ServerSetStaminaLevel(ABaseCharacter* ComponentOwner, float CurrentStamina, float MaxStamina);

	FORCEINLINE UStaminaComponent* GetStaminaComponent() const { return StaminaComponent; }
	FORCEINLINE UHealthComponent* GetHealthComponent() const { return HealthComponent; }

protected:
	virtual void BeginPlay() override;
	virtual void Tick(float DeltaSeconds) override;
	virtual void OnMovementModeChanged(EMovementMode PrevMovementMode, uint8 PreviousCustomMode) override;

	/** Change Movement State to Sprint or Walk based on previous Movement State */
	UFUNCTION(Server, Reliable)
	void ServerChangeMovementState(EMovementState NewMovementState);

	UFUNCTION(Server, Reliable)
	void ServerInteractWithWeapon();

	UFUNCTION(Server, Reliable, WithValidation)
	void ServerInteractWithAmmo();
	
	UFUNCTION(Server, Reliable, WithValidation)
	void ServerInteractWithHealth();
	
	virtual void ClientUpdateHealth_Implementation(float NewHealth);
	virtual void ClientUpdateStamina_Implementation(float NewStamina);

private:
	UFUNCTION(Server, Reliable)
	void ServerCheckMovementMode(EMovementMode PrevMovementMode);
	void ServerCheckMovementMode_Implementation(EMovementMode PrevMovementMode);
	
	void ServerChangeMovementState_Implementation(EMovementState NewMovementState);

	void ServerInteractWithWeapon_Implementation();

	bool ServerInteractWithAmmo_Validate();
	void ServerInteractWithAmmo_Implementation();

	bool ServerInteractWithHealth_Validate();
	void ServerInteractWithHealth_Implementation();

	UFUNCTION(Server, Reliable)
	void ServerAddWeapon(AWeaponPickupActor* NewWeapon);
	void ServerAddWeapon_Implementation(AWeaponPickupActor* NewWeapon);

	UFUNCTION(Server, Reliable)
	void ServerUpdateCurrentWeapon(AWeaponPickupActor* NewWeapon, EWeaponToDo WeaponToEquip);
	void ServerUpdateCurrentWeapon_Implementation(AWeaponPickupActor* NewWeapon, EWeaponToDo WeaponToEquip);
	
	UFUNCTION(Server, Reliable)
	void ServerDropWeapon(EWeaponToDo WeaponToDrop);
	void ServerDropWeapon_Implementation(EWeaponToDo WeaponToDrop);
	
	void ServerSetHealthLevel_Implementation(ABaseCharacter* ComponentOwner, float CurrentHealth, float MaxHealth);
	void ServerSetStaminaLevel_Implementation(ABaseCharacter* ComponentOwner, float CurrentStamina, float MaxStamina);
	
	/** Update health on player UI */
	UFUNCTION(Client, Unreliable)
	void ClientUpdateHealth(float NewHealth);

	/** Update stamina on player UI */
	UFUNCTION(Client, Unreliable)
	void ClientUpdateStamina(float NewStamina);

	UFUNCTION(NetMulticast, Reliable)
	void MulticastDeath();
	void MulticastDeath_Implementation();

	UFUNCTION(Server, Reliable)
	void ServerStartDestroy();
	void ServerStartDestroy_Implementation();
	
// Variables
protected:
	UPROPERTY(Replicated)
	EMovementState MovementState;

	UPROPERTY(Replicated)
	float MovementScale;
	
	/** To check only once if character is moving or not */
	uint8 bDoOnceMoving : 1, bDoOnceStopped : 1;

private:
	UPROPERTY(Replicated, EditDefaultsOnly, Category = "Defaults", meta = (ClampMin = "0.0", UIMin = "0.0", AllowPrivateAccess = true))
	float RespawnDelay;
};