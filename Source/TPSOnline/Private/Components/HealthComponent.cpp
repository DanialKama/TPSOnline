// All Rights Reserved.

#include "Components/HealthComponent.h"
#include "Characters/BaseCharacter.h"
#include "Net/UnrealNetwork.h"

UHealthComponent::UHealthComponent()
{
	PrimaryComponentTick.bCanEverTick = false;

	// Initialize variables
	MaxHealth = 100.0f;
	RestoreAmount = 5.0f;
	RestoreDelay = 2.0f;
}

void UHealthComponent::GetLifetimeReplicatedProps(TArray<FLifetimeProperty> &OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	// Replicate to everyone
	DOREPLIFETIME(UHealthComponent, CurrentHealth);
	DOREPLIFETIME(UHealthComponent, MaxHealth);
	DOREPLIFETIME(UHealthComponent, RestoreAmount);
	DOREPLIFETIME(UHealthComponent, RestoreDelay);
}

void UHealthComponent::ServerInitialize_Implementation(UBaseComponent* Self)
{
	if (GetOwnerRole() == ROLE_Authority)
	{
		Super::ServerInitialize_Implementation(Self);

		CurrentHealth = MaxHealth;
		ComponentOwner->OnTakeAnyDamage.AddDynamic(this, &UHealthComponent::TakeAnyDamage);
	}
}

void UHealthComponent::TakeAnyDamage(AActor* DamagedActor, float Damage, const UDamageType* DamageType, AController* InstigatedBy, AActor* DamageCauser)
{
	if (Damage > 0.0f)
	{
		GetWorld()->GetTimerManager().ClearTimer(RestoreHealth);
		CurrentHealth = FMath::Clamp(CurrentHealth - Damage, 0.0f, MaxHealth);
		ComponentOwner->ServerSetHealthLevel(CurrentHealth / MaxHealth);
	}
}

bool UHealthComponent::ServerIncreaseHealth_Validate(float IncreaseAmount)
{
	if (IncreaseAmount > 0.0f)
	{
		return true;
	}
	return false;
}

void UHealthComponent::ServerIncreaseHealth_Implementation(float IncreaseAmount)
{
	if (GetOwnerRole() == ROLE_Authority)
	{
		CurrentHealth = FMath::Clamp(CurrentHealth + IncreaseAmount, 0.0f, MaxHealth);
		ComponentOwner->ServerSetHealthLevel(CurrentHealth / MaxHealth);
	}
}