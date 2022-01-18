// All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "Components/BaseComponent.h"
#include "HealthComponent.generated.h"

UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class TPSONLINE_API UHealthComponent : public UBaseComponent
{
	GENERATED_BODY()

// Functions
public:	
	/** Sets default values for this component's properties */
	UHealthComponent();
	
	UFUNCTION(Server, Reliable, WithValidation)
	void ServerIncreaseHealth(float IncreaseAmount);
	
private:
	virtual void ServerInitialize_Implementation(UBaseComponent* Self) override;

	UFUNCTION()
	void TakeAnyDamage(AActor* DamagedActor, float Damage, const UDamageType* DamageType, AController* InstigatedBy, AActor* DamageCauser);
	
	bool ServerIncreaseHealth_Validate(float IncreaseAmount);
	void ServerIncreaseHealth_Implementation(float IncreaseAmount);

// Variables
public:
	UPROPERTY(Replicated, EditAnywhere, Category = "Defaults", meta = (ClampMin = "0.0", UIMin = "0.0", AllowPrivateAccess = true))
	float MaxHealth;
	
	UPROPERTY(Replicated)
	float CurrentHealth;
	
private:
	UPROPERTY(Replicated, EditAnywhere, Category = "Defaults", meta = (ClampMin = "0.0", UIMin = "0.0", AllowPrivateAccess = true))
	float RestoreAmount;

	UPROPERTY(Replicated, EditAnywhere, Category = "Defaults", meta = (ClampMin = "0.0", UIMin = "0.0", AllowPrivateAccess = true))
	float RestoreDelay;

	FTimerHandle RestoreHealth;
};