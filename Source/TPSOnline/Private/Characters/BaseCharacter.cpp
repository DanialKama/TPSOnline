// All Rights Reserved.

#include "Characters/BaseCharacter.h"
#include "Components/HealthComponent.h"
#include "Components/StaminaComponent.h"
#include "GameFramework/CharacterMovementComponent.h"

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

void ABaseCharacter::SetStaminaLevel(float CurrentStamina)
{
	if (CurrentStamina <= 0.0f)
	{
		GetCharacterMovement()->MaxWalkSpeed = 240.0f;
		GetCharacterMovement()->JumpZVelocity = 300.0f;
	}
}