// All Rights Reserved.

#include "Characters/BaseCharacter.h"
#include "Components/HealthComponent.h"
#include "Components/StaminaComponent.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "Net/UnrealNetwork.h"

ABaseCharacter::ABaseCharacter()
{
	PrimaryActorTick.bCanEverTick = true;
	
	HealthComponent = CreateDefaultSubobject<UHealthComponent>(TEXT("Health Component"));
	StaminaComponent = CreateDefaultSubobject<UStaminaComponent>(TEXT("Stamina Component"));

	bDoOnceMoving = true;
	bDoOnceStopped = true;
}

void ABaseCharacter::BeginPlay()
{
	Super::BeginPlay();
	
	HealthComponent->Initialize();
	StaminaComponent->Initialize();
}

void ABaseCharacter::Tick(float DeltaSeconds)
{
	Super::Tick(DeltaSeconds);

	// Check if character stopped or moving
	if (GetVelocity().Size() == 0.0f)
	{
		if (bDoOnceStopped)
		{
			bDoOnceMoving = true;
			bDoOnceStopped = false;

			if (MovementState == EMovementState::Run || MovementState == EMovementState::Sprint)
			{
				StaminaComponent->StopStaminaDrain();
			}
		}

	}
	else if (bDoOnceMoving)
	{
		bDoOnceMoving = false;
		bDoOnceStopped = true;

		if (MovementState == EMovementState::Run || MovementState == EMovementState::Sprint)
		{
			StaminaComponent->StartStaminaDrain(MovementState);
		}
	}
}

void ABaseCharacter::GetLifetimeReplicatedProps(TArray<FLifetimeProperty> &OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	// Replicate to everyone
	DOREPLIFETIME(ABaseCharacter, MovementState);
}

bool ABaseCharacter::ServerToggleSprint_Validate(EMovementState NewMovementState)
{
	return true;
}

void ABaseCharacter::ServerToggleSprint_Implementation(EMovementState NewMovementState)	// TODO - Fix stutter effect
{
	if (HasAuthority())
	{
		MovementState = NewMovementState;
		switch (NewMovementState)
		{
		case 0:
			// Walk
			StaminaComponent->StopStaminaDrain();
			GetCharacterMovement()->MaxWalkSpeed = 150.0f;
			GetCharacterMovement()->MaxWalkSpeedCrouched = 150.0f;
			GetCharacterMovement()->JumpZVelocity = 300.0f;
			break;
		case 1:
			// Run
			if (StaminaComponent->CurrentStamina > 0.0f)
			{
				UnCrouch();
				GetCharacterMovement()->MaxWalkSpeed = 300.0f;
				GetCharacterMovement()->MaxWalkSpeedCrouched = 300.0f;
				GetCharacterMovement()->JumpZVelocity = 360.0f;
			}
			break;
		case 2:
			// Sprint
			if (StaminaComponent->CurrentStamina > 0.0f)
			{
				UnCrouch();
				GetCharacterMovement()->MaxWalkSpeed = 600.0f;
				GetCharacterMovement()->MaxWalkSpeedCrouched = 600.0f;
				GetCharacterMovement()->JumpZVelocity = 420.0f;
			}
			break;
		case 3:
			// Crouch
			if (GetCharacterMovement()->IsFalling())
			{
				UnCrouch();
			}
			else
			{
				Crouch();
			}
			StaminaComponent->StopStaminaDrain();
			GetCharacterMovement()->MaxWalkSpeed = 150.0f;
			GetCharacterMovement()->MaxWalkSpeedCrouched = 150.0f;
			GetCharacterMovement()->JumpZVelocity = 0.0f;
			break;
		case 4:
			// Prone
			StaminaComponent->StopStaminaDrain();
			GetCharacterMovement()->MaxWalkSpeed = 80.0f;
			GetCharacterMovement()->MaxWalkSpeedCrouched = 80.0f;
			GetCharacterMovement()->JumpZVelocity = 0.0f;
			break;
		}
	}
	else
	{
		ServerToggleSprint(NewMovementState);
	}
}

void ABaseCharacter::SetHealthLevel(float CurrentHealth)
{
	if (CurrentHealth <= 0.0f)
	{
		GetCharacterMovement()->DisableMovement();
		GetMesh()->SetCollisionProfileName(FName("Ragdoll"), true);
		GetMesh()->SetSimulatePhysics(true);
	}
}

void ABaseCharacter::SetStaminaLevel(float CurrentStamina)
{
	if (CurrentStamina <= 0.0f)
	{
		GetCharacterMovement()->MaxWalkSpeed = 240.0f;
		GetCharacterMovement()->JumpZVelocity = 300.0f;
	}
}