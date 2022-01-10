// All Rights Reserved.

#include "Components/StaminaComponent.h"
#include "Components/HealthComponent.h"
#include "Net/UnrealNetwork.h"

UStaminaComponent::UStaminaComponent()
{
	PrimaryComponentTick.bCanEverTick = false;
	SetIsReplicated(true);

	MaxStamina = 100.0f;
	RunningDrainAmount = 1.5f;
	SprintingDrainAmount = 3.0f;
	RestoreStaminaDelay = 2.0f;
}

void UStaminaComponent::Initialize()
{
	Super::Initialize();
	
	CurrentStamina = MaxStamina;
	CompOwner->SetStaminaLevel(CurrentStamina / MaxStamina);
}

void UStaminaComponent::GetLifetimeReplicatedProps(TArray<FLifetimeProperty> &OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	// Replicate to everyone
	DOREPLIFETIME(UStaminaComponent, CurrentStamina);
}

void UStaminaComponent::StartStaminaDrain(EMovementState MovementState)
{
	if (GetOwnerRole() == ROLE_Authority)
	{
		GetWorld()->GetTimerManager().ClearTimer(DrainStaminaTimer);
		
		if (MovementState == EMovementState::Run)
		{
			GetWorld()->GetTimerManager().SetTimer(DrainStaminaTimer, this, &UStaminaComponent::RunningDrainStamina, 0.2f, true);
		}
		else if (MovementState == EMovementState::Sprint)
		{
			GetWorld()->GetTimerManager().SetTimer(DrainStaminaTimer, this, &UStaminaComponent::SprintingDrainStamina, 0.2f, true);
		}
	}
}

void UStaminaComponent::StopStaminaDrain()
{
	if (GetOwnerRole() == ROLE_Authority)
	{
		GetWorld()->GetTimerManager().ClearTimer(DrainStaminaTimer);
	}
}

void UStaminaComponent::RunningDrainStamina()
{
	if (CurrentStamina <= 0.0f)
	{
		GetWorld()->GetTimerManager().ClearTimer(DrainStaminaTimer);
	}
	else
	{
		CurrentStamina = FMath::Clamp(CurrentStamina - RunningDrainAmount, 0.0f, MaxStamina);
	}
	
	CompOwner->SetStaminaLevel(CurrentStamina / MaxStamina);
}

bool UStaminaComponent::ServerRunningDrainStamina_Validate()
{
	return true;
}

void UStaminaComponent::ServerRunningDrainStamina_Implementation()
{
	if (GetOwnerRole() == ROLE_Authority)
	{
		RunningDrainStamina();
	}
}

void UStaminaComponent::SprintingDrainStamina()
{
	if (GetOwnerRole() == ROLE_Authority)
	{
		if (CurrentStamina <= 0.0f)
		{
			GetWorld()->GetTimerManager().ClearTimer(DrainStaminaTimer);
		}
		else
		{
			CurrentStamina = FMath::Clamp(CurrentStamina - SprintingDrainAmount, 0.0f, MaxStamina);
		}
	}
	else
	{
		ServerSprintingDrainStamina();
	}

	CompOwner->SetStaminaLevel(CurrentStamina / MaxStamina);
}

bool UStaminaComponent::ServerSprintingDrainStamina_Validate()
{
	return true;
}

void UStaminaComponent::ServerSprintingDrainStamina_Implementation()
{
	if (GetOwnerRole() == ROLE_Authority)
	{
		SprintingDrainStamina();
	}
}