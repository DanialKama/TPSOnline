// All Rights Reserved.

#include "Components/HealthComponent.h"
#include "Characters/BaseCharacter.h"
#include "Net/UnrealNetwork.h"

UHealthComponent::UHealthComponent()
{
	PrimaryComponentTick.bCanEverTick = false;
	SetIsReplicated(true);

	MaxHealth = 100.0f;
	RestoreAmount = 5.0f;
	RestoreDelay = 2.0f;
}

void UHealthComponent::Initialize()
{
	Super::Initialize();

	CurrentHealth = MaxHealth;
	ComponentOwner->OnTakeAnyDamage.AddDynamic(this, &UHealthComponent::TakeAnyDamage);
}

void UHealthComponent::GetLifetimeReplicatedProps(TArray<FLifetimeProperty> &OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	// Replicate to everyone
	DOREPLIFETIME(UHealthComponent, CurrentHealth);
}

void UHealthComponent::TakeAnyDamage(AActor* DamagedActor, float Damage, const UDamageType* DamageType, AController* InstigatedBy, AActor* DamageCauser)
{
	if (Damage > 0.0f)
	{
		CurrentHealth = FMath::Clamp(CurrentHealth - Damage, 0.0f, MaxHealth);
	}

	ComponentOwner->SetHealthLevel(CurrentHealth);
}