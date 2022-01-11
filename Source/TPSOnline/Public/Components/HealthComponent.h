// All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "Components/BaseComponent.h"
#include "HealthComponent.generated.h"

UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class TPSONLINE_API UHealthComponent : public UBaseComponent
{
	GENERATED_BODY()

public:	
	/** Sets default values for this component's properties */
	UHealthComponent();

	virtual void Initialize() override;
	
private:
	UFUNCTION()
	void TakeAnyDamage(AActor* DamagedActor, float Damage, const UDamageType* DamageType, AController* InstigatedBy, AActor* DamageCauser);

	/** Update health level on player UI */
	UFUNCTION(Client, Unreliable)
	void ClientUpdateHealth();

public:
	UPROPERTY(Replicated)
	float CurrentHealth;
	
private:
	UPROPERTY(Replicated, EditAnywhere, Category = "Defaults", meta = (AllowPrivateAccess = true))
	float MaxHealth;

	UPROPERTY(Replicated, EditAnywhere, Category = "Defaults", meta = (AllowPrivateAccess = true))
	float RestoreAmount;

	UPROPERTY(Replicated, EditAnywhere, Category = "Defaults", meta = (AllowPrivateAccess = true))
	float RestoreDelay;
};