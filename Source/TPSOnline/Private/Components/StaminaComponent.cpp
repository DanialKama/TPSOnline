// Copyright 2022 Danial Kamali. All Rights Reserved.

#include "Components/StaminaComponent.h"
#include "Components/HealthComponent.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "Net/UnrealNetwork.h"

UStaminaComponent::UStaminaComponent()
{
	PrimaryComponentTick.bCanEverTick = false;

	// Initialize variables
	CurrentStamina = MaxStamina = 100.0f;
	RunningDrainAmount = 1.5f;
	SprintingDrainAmount = 3.0f;
	JumpingDrainAmount = 10.0f;
	RestoreStaminaAmount = 5.0f;
	RestoreStaminaDelay = 2.0f;
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

void UStaminaComponent::ServerInitialize_Implementation()
{
	Super::ServerInitialize_Implementation();

	CurrentStamina = MaxStamina;
	ComponentOwner->ServerSetStaminaLevel(CurrentStamina, MaxStamina);
}

void UStaminaComponent::ServerStartStaminaDrain_Implementation(EMovementState MovementState)
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

bool UStaminaComponent::ServerRunningDrainStamina_Validate()
{
	if (ComponentOwner->GetCharacterMovement()->MovementMode == MOVE_Falling || ComponentOwner->GetVelocity().Size() == 0.0f)
	{
		ServerStopStaminaDrain(false);
		return false;
	}
	return true;
}

void UStaminaComponent::ServerRunningDrainStamina_Implementation()
{
	if (CurrentStamina <= 0.0f)
	{
		GetWorld()->GetTimerManager().ClearTimer(DrainStaminaTimer);
	}
	else
	{
		CurrentStamina = FMath::Clamp(CurrentStamina - RunningDrainAmount, 0.0f, MaxStamina);
		ComponentOwner->ServerSetStaminaLevel(CurrentStamina, MaxStamina);
	}
}

bool UStaminaComponent::ServerSprintingDrainStamina_Validate()
{
	if (ComponentOwner->GetCharacterMovement()->MovementMode == MOVE_Falling || ComponentOwner->GetVelocity().Size() == 0.0f)
	{
		ServerStopStaminaDrain(false);
		return false;
	}
	return true;
}

void UStaminaComponent::ServerSprintingDrainStamina_Implementation()
{
	if (CurrentStamina <= 0.0f)
	{
		GetWorld()->GetTimerManager().ClearTimer(DrainStaminaTimer);
	}
	else
	{
		CurrentStamina = FMath::Clamp(CurrentStamina - SprintingDrainAmount, 0.0f, MaxStamina);
		ComponentOwner->ServerSetStaminaLevel(CurrentStamina, MaxStamina);
	}
}

void UStaminaComponent::ServerStopStaminaDrain_Implementation(bool bStartRestore)
{
	GetWorld()->GetTimerManager().ClearTimer(DrainStaminaTimer);

	// If stamina is currently not restoring
	if (bStartRestore && RestoreStaminaTimer.IsValid() == false)
	{
		GetWorld()->GetTimerManager().SetTimer(RestoreStaminaTimer, this, &UStaminaComponent::ServerRestoreStamina, 0.2f, true, RestoreStaminaDelay);
	}
}

void UStaminaComponent::ServerRestoreStamina_Implementation()
{
	if (CurrentStamina >= MaxStamina)
	{
		GetWorld()->GetTimerManager().ClearTimer(RestoreStaminaTimer);
	}
	else
	{
		CurrentStamina = FMath::Clamp(CurrentStamina + RestoreStaminaAmount, 0.0f, MaxStamina);
		ComponentOwner->ServerSetStaminaLevel(CurrentStamina, MaxStamina);
	}
}

void UStaminaComponent::ServerJumpDrainStamina_Implementation()
{
	GetWorld()->GetTimerManager().ClearTimer(RestoreStaminaTimer);
	GetWorld()->GetTimerManager().ClearTimer(DrainStaminaTimer);
	CurrentStamina = FMath::Clamp(CurrentStamina - JumpingDrainAmount, 0.0f, MaxStamina);
	ComponentOwner->ServerSetStaminaLevel(CurrentStamina, MaxStamina);
}