// Copyright 2022 Danial Kamali. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "Enumerations/CharacterEnums.h"
#include "GameFramework/Character.h"
#include "BaseCharacter.generated.h"

class APickupActor;
class AWeaponPickupActor;

UCLASS()
class ABaseCharacter : public ACharacter
{
	GENERATED_BODY()

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components", meta = (AllowPrivateAccess = "true"))
	TObjectPtr<class UHealthComponent> HealthComponent;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components", meta = (AllowPrivateAccess = "true"))
	TObjectPtr<class UStaminaComponent> StaminaComponent;

// Functions
public:
	ABaseCharacter();

	/** Use a single trace to find if the hit actor is a pickup */
	APickupActor* FindPickup() const;

	UFUNCTION(Server, Reliable)
	void ServerSetHealthLevel(float CurrentHealth, float MaxHealth);

	UFUNCTION(Server, Reliable)
	void ServerSetStaminaLevel(float CurrentStamina, float MaxStamina);
	
	FORCEINLINE UStaminaComponent* GetStaminaComponent() const { return StaminaComponent; }
	FORCEINLINE UHealthComponent* GetHealthComponent() const { return HealthComponent; }

protected:
	virtual void BeginPlay() override;
	virtual void PossessedBy(AController* NewController) override;
	virtual void Tick(float DeltaSeconds) override;
	virtual void OnMovementModeChanged(EMovementMode PrevMovementMode, uint8 PreviousCustomMode) override;

	/** Change Movement State to Sprint or Walk based on previous Movement State */
	UFUNCTION(Server, Reliable)
	void ServerChangeMovementState(EMovementState NewMovementState);

	UFUNCTION(Server, Reliable)
	void ServerInteractWithWeapon();

	UFUNCTION(Server, Reliable)
	void ServerInteractWithAmmo();
	
	UFUNCTION(Server, Reliable, WithValidation)
	void ServerInteractWithHealth();

	UFUNCTION(Server, Reliable)
	void ServerUpdateAimState(bool bAim);

	UFUNCTION(Server, Reliable, WithValidation)
	void ServerStartFireWeapon();

	UFUNCTION(Server, Reliable, WithValidation)
	void ServerStopFireWeapon();

	/** Checking if there is any ammo for this weapon. */
	bool CanFireWeapon() const;

	/** Checking if there is any ammo in inventory. */
	bool CanReloadWeapon() const;

	UFUNCTION(Server, Reliable, WithValidation)
	void ServerReloadWeapon();
	
	UFUNCTION(Server, Reliable, WithValidation)
	void ServerDropWeapon(EWeaponToDo WeaponToDrop);

	UFUNCTION()
	virtual void OnRep_CurrentWeapon();
	
	virtual void ClientUpdateHealth_Implementation(float NewHealth);
	virtual void ClientUpdateStamina_Implementation(float NewStamina);

private:
	UFUNCTION(Server, Reliable)
	void ServerCheckMovementMode(EMovementMode PrevMovementMode);
	void ServerCheckMovementMode_Implementation(EMovementMode PrevMovementMode);
	
	void ServerChangeMovementState_Implementation(EMovementState NewMovementState);

	UFUNCTION()
	void OnRep_MovementState();

	UFUNCTION()
	void OnRep_IsAiming() const;
	
	void ServerInteractWithWeapon_Implementation();

	void ServerInteractWithAmmo_Implementation();

	bool ServerInteractWithHealth_Validate();
	void ServerInteractWithHealth_Implementation();

	void ServerUpdateAimState_Implementation(bool bAim);

	bool ServerStartFireWeapon_Validate();
	void ServerStartFireWeapon_Implementation();

	bool ServerStopFireWeapon_Validate();
	void ServerStopFireWeapon_Implementation();

	UFUNCTION(Server, Reliable, WithValidation)
	void ServerFireWeapon();
	bool ServerFireWeapon_Validate();
	void ServerFireWeapon_Implementation();

	/** Add a delay to stop the player from firing faster than the weapon's fire rate */
	UFUNCTION(Server, Reliable)
	void ServerResetFireWeapon();
	void ServerResetFireWeapon_Implementation();

	bool ServerReloadWeapon_Validate();
	void ServerReloadWeapon_Implementation();
	
	UFUNCTION(Server, Reliable, WithValidation)
	void ServerAddWeapon(AWeaponPickupActor* NewWeapon);
	bool ServerAddWeapon_Validate(AWeaponPickupActor* NewWeapon);
	void ServerAddWeapon_Implementation(AWeaponPickupActor* NewWeapon);

	UFUNCTION(Server, Reliable)
	void ServerUpdateCurrentWeapon(AWeaponPickupActor* NewWeapon, EWeaponToDo WeaponToEquip);
	void ServerUpdateCurrentWeapon_Implementation(AWeaponPickupActor* NewWeapon, EWeaponToDo WeaponToEquip);
	
	bool ServerDropWeapon_Validate(EWeaponToDo WeaponToDrop);
	void ServerDropWeapon_Implementation(EWeaponToDo WeaponToDrop);
	
	void ServerSetHealthLevel_Implementation(float CurrentHealth, float MaxHealth);
	void ServerSetStaminaLevel_Implementation(float CurrentStamina, float MaxStamina);
	
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
	UPROPERTY(ReplicatedUsing = "OnRep_MovementState", BlueprintReadOnly, Category = "Defaults")
	EMovementState MovementState;

	UPROPERTY(Replicated)
	float MovementScale;

	UPROPERTY(Replicated)
	class ACustomPlayerState* PlayerStateRef;

	/** The weapon that is currently in the player's hand */
	UPROPERTY(ReplicatedUsing = "OnRep_CurrentWeapon", BlueprintReadOnly, Category = "Defaults")
	TObjectPtr<AWeaponPickupActor> CurrentWeapon;

	UPROPERTY(Replicated)
	EWeaponToDo CurrentWeaponSlot;

	UPROPERTY(ReplicatedUsing = "OnRep_IsAiming", BlueprintReadOnly, Category = "Defaults", meta = (AllowPrivateAccess = true))
	uint8 bIsAiming : 1;

	/** To call Multicast Death only once */
	UPROPERTY(Replicated)
	uint8 bDoOnceDeath : 1;

	UPROPERTY(Replicated)
	uint8 bCanFireWeapon : 1;

	UPROPERTY(Replicated)
	uint8 bDoOnceReload : 1;
	
	/** To check only once if character is moving or not */
	uint8 bDoOnceMoving : 1, bDoOnceStopped : 1;

	/** Use as a reference for projectile line trace. */
	UPROPERTY()
	TObjectPtr<class UCameraComponent> CurrentCamera;

private:
	UPROPERTY(Replicated, EditDefaultsOnly, Category = "Defaults", meta = (ClampMin = "0.0", UIMin = "0.0", AllowPrivateAccess = true))
	float RespawnDelay;

	UPROPERTY(Replicated, BlueprintReadOnly, Category = "Defaults", meta = (AllowPrivateAccess = true))
	uint8 bIsArmed : 1;
	
	FTimerHandle FireWeaponTimer, ResetFireWeaponTimer;
};
