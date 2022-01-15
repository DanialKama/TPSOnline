// All Rights Reserved.

#include "Components/StaminaComponent.h"
#include "Components/HealthComponent.h"
#include "Net/UnrealNetwork.h"

UStaminaComponent::UStaminaComponent()
{
	PrimaryComponentTick.bCanEverTick = false;
	
	MaxStamina = 100.0f;
	RunningDrainAmount = 1.5f;
	SprintingDrainAmount = 3.0f;
	JumpingDrainAmount = 10.0f;
	RestoreStaminaAmount = 5.0f;
	RestoreStaminaDelay = 2.0f;
}

void UStaminaComponent::Initialize()
{
	Super::Initialize();
	
	CurrentStamina = MaxStamina;
	ComponentOwner->SetStaminaLevel(CurrentStamina / MaxStamina);
}

void UStaminaComponent::GetLifetimeReplicatedProps(TArray<FLifetimeProperty> &OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	// Replicate to everyone
	DOREPLIFETIME(UStaminaComponent, CurrentStamina);
	DOREPLIFETIME(UStaminaComponent, MaxStamina);
	DOREPLIFETIME(UStaminaComponent, RunningDrainAmount);
	DOREPLIFETIME(UStaminaComponent, SprintingDrainAmount);
	DOREPLIFETIME(UStaminaComponent, JumpingDrainAmount);
	DOREPLIFETIME(UStaminaComponent, RestoreStaminaAmount);
	DOREPLIFETIME(UStaminaComponent, RestoreStaminaDelay);
}

bool UStaminaComponent::ServerStartStaminaDrain_Validate(EMovementState MovementState)
{
	return true;
}

void UStaminaComponent::ServerStartStaminaDrain_Implementation(EMovementState MovementState)
{
	if (GetOwnerRole() == ROLE_Authority)
	{
		GetWorld()->GetTimerManager().ClearTimer(RestoreStaminaTimer);
		GetWorld()->GetTimerManager().ClearTimer(DrainStaminaTimer);
		
		if (MovementState == EMovementState::Run)
		{
			GetWorld()->GetTimerManager().SetTimer(DrainStaminaTimer, this, &UStaminaComponent::ServerRunningDrainStamina, 0.2f, true);
		}
		else if (MovementState == EMovementState::Sprint)
		{
			GetWorld()->GetTimerManager().SetTimer(DrainStaminaTimer, this, &UStaminaComponent::ServerSprintingDrainStamina, 0.2f, true);
		}
	}
	else
	{
		ServerStartStaminaDrain(MovementState);
	}
}

bool UStaminaComponent::ServerRunningDrainStamina_Validate()
{
	return true;
}

void UStaminaComponent::ServerRunningDrainStamina_Implementation()
{
	if (GetOwnerRole() == ROLE_Authority)
	{
		if (CurrentStamina <= 0.0f)
		{
			GetWorld()->GetTimerManager().ClearTimer(DrainStaminaTimer);
		}
		else
		{
			CurrentStamina = FMath::Clamp(CurrentStamina - RunningDrainAmount, 0.0f, MaxStamina);
		}
	
		ClientUpdateStamina(CurrentStamina / MaxStamina);
	}
	else
	{
		ServerRunningDrainStamina();
	}
}

bool UStaminaComponent::ServerSprintingDrainStamina_Validate()
{
	return true;
}

void UStaminaComponent::ServerSprintingDrainStamina_Implementation()
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

		ClientUpdateStamina(CurrentStamina / MaxStamina);
	}
	else
	{
		ServerSprintingDrainStamina();
	}
}

bool UStaminaComponent::ServerStopStaminaDrain_Validate()
{
	return true;
}

void UStaminaComponent::ServerStopStaminaDrain_Implementation()
{
	if (GetOwnerRole() == ROLE_Authority)
	{
		GetWorld()->GetTimerManager().ClearTimer(DrainStaminaTimer);

		// If timer is invalid
		if (!RestoreStaminaTimer.IsValid())
		{
			GetWorld()->GetTimerManager().SetTimer(RestoreStaminaTimer, this, &UStaminaComponent::ServerRestoreStamina, 0.2f, true, RestoreStaminaDelay);
		}
	}
	else
	{
		ServerStopStaminaDrain();
	}
}

bool UStaminaComponent::ServerRestoreStamina_Validate()
{
	return true;
}

void UStaminaComponent::ServerRestoreStamina_Implementation()
{
	if (GetOwnerRole() == ROLE_Authority)
	{
		if (CurrentStamina == MaxStamina)
		{
			GetWorld()->GetTimerManager().ClearTimer(RestoreStaminaTimer);
		}
		else
		{
			CurrentStamina = FMath::Clamp(CurrentStamina + RestoreStaminaAmount, 0.0f, MaxStamina);
		}

		ClientUpdateStamina(CurrentStamina / MaxStamina);
	}
	else
	{
		ServerRestoreStamina();
	}
}

bool UStaminaComponent::ServerJumpDrainStamina_Validate()
{
	return true;
}

void UStaminaComponent::ServerJumpDrainStamina_Implementation()
{
	if (GetOwnerRole() == ROLE_Authority)
	{
		CurrentStamina = FMath::Clamp(CurrentStamina - JumpingDrainAmount, 0.0f, MaxStamina);
	}
	else
	{
		ServerJumpDrainStamina();
	}
}

void UStaminaComponent::ClientUpdateStamina_Implementation(float NewStamina)
{
	ComponentOwner->SetStaminaLevel(NewStamina);
}