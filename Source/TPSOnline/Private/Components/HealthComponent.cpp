// All Rights Reserved.

#include "Components/HealthComponent.h"
#include "Characters/BaseCharacter.h"
#include "Components/StaminaComponent.h"
#include "Net/UnrealNetwork.h"

UHealthComponent::UHealthComponent()
{
	PrimaryComponentTick.bCanEverTick = false;

	// Initialize variables
	CurrentHealth = MaxHealth = 100.0f;
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
		ComponentOwner->ServerSetHealthLevel(ComponentOwner, CurrentHealth, MaxHealth);
		ComponentOwner->OnTakeAnyDamage.AddDynamic(this, &UHealthComponent::TakeAnyDamage);
	}
}

void UHealthComponent::TakeAnyDamage(AActor* DamagedActor, float Damage, const UDamageType* DamageType, AController* InstigatedBy, AActor* DamageCauser)
{
	if (Damage > 0.0f)
	{
		GetWorld()->GetTimerManager().ClearTimer(RestoreHealthTimer);
		CurrentHealth = FMath::Clamp(CurrentHealth - Damage, 0.0f, MaxHealth);
		ComponentOwner->ServerSetHealthLevel(ComponentOwner, CurrentHealth, MaxHealth);
	}
}

void UHealthComponent::ServerIncreaseHealth_Implementation(float IncreaseAmount)
{
	if (GetOwnerRole() == ROLE_Authority)
	{
		CurrentHealth = FMath::Clamp(CurrentHealth + IncreaseAmount, 0.0f, MaxHealth);
		ComponentOwner->ServerSetHealthLevel(ComponentOwner, CurrentHealth, MaxHealth);
	}
}

void UHealthComponent::ServerStartRestoreHealth_Implementation()
{
	if (GetOwnerRole() == ROLE_Authority)
	{
		GetWorld()->GetTimerManager().SetTimer(RestoreHealthTimer, this, &UHealthComponent::ServerRestoreHealth, 0.2f, true, RestoreDelay);
	}
}

bool UHealthComponent::ServerRestoreHealth_Validate()
{
	if (CurrentHealth >= MaxHealth && ComponentOwner->GetStaminaComponent()->CurrentStamina < ComponentOwner->GetStaminaComponent()->MaxStamina)
	{
		ServerStopRestoreHealth();
		return false;
	}
	return true;
}

void UHealthComponent::ServerRestoreHealth_Implementation()
{
	if (GetOwnerRole() == ROLE_Authority)
	{
		if (CurrentHealth >= MaxHealth)
		{
			ServerStopRestoreHealth();
		}
		else
		{
			CurrentHealth = FMath::Clamp(CurrentHealth + RestoreAmount, 0.0f, MaxHealth);
			ComponentOwner->ServerSetHealthLevel(ComponentOwner, CurrentHealth, MaxHealth);
		}
	}
}

void UHealthComponent::ServerStopRestoreHealth_Implementation()
{
	if (GetOwnerRole() == ROLE_Authority)
	{
		GetWorld()->GetTimerManager().ClearTimer(RestoreHealthTimer);
	}	
}