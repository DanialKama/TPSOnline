// All Rights Reserved.

#include "Characters/BaseCharacter.h"
#include "Components/CapsuleComponent.h"
#include "Actors/PickupActor.h"
#include "Actors/HealthPickupActor.h"
#include "Components/HealthComponent.h"
#include "Components/StaminaComponent.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "Kismet/KismetSystemLibrary.h"
#include "Net/UnrealNetwork.h"

ABaseCharacter::ABaseCharacter()
{
	PrimaryActorTick.bCanEverTick = true;
	
	HealthComponent = CreateDefaultSubobject<UHealthComponent>(TEXT("Health Component"));
	StaminaComponent = CreateDefaultSubobject<UStaminaComponent>(TEXT("Stamina Component"));

	// Initialize variables
	bDoOnceMoving = true;
	bDoOnceStopped = true;
}

void ABaseCharacter::GetLifetimeReplicatedProps(TArray<FLifetimeProperty> &OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	// Replicate to everyone
	DOREPLIFETIME(ABaseCharacter, MovementState);
	DOREPLIFETIME(ABaseCharacter, MovementScale);
}

void ABaseCharacter::BeginPlay()
{
	Super::BeginPlay();

	if (GetLocalRole() == ROLE_Authority)
	{
		ServerChangeMovementState(EMovementState::Walk);
		HealthComponent->ServerInitialize(HealthComponent);
		StaminaComponent->ServerInitialize(StaminaComponent);
	}
}

void ABaseCharacter::Tick(float DeltaSeconds)
{
	Super::Tick(DeltaSeconds);

	// Check if character stopped or moving
	if (GetVelocity().Size() == 0.0f) // TODO - Find another way and do not use Tick
	{
		if (bDoOnceStopped)
		{
			bDoOnceMoving = true;
			bDoOnceStopped = false;

			if (MovementState == EMovementState::Run || MovementState == EMovementState::Sprint)
			{
				StaminaComponent->ServerStopStaminaDrain();
			}
		}
	}
	else if (bDoOnceMoving)
	{
		bDoOnceMoving = false;
		bDoOnceStopped = true;

		if (MovementState == EMovementState::Run || MovementState == EMovementState::Sprint)
		{
			StaminaComponent->ServerStartStaminaDrain(MovementState);
		}
	}
}

void ABaseCharacter::OnMovementModeChanged(EMovementMode PrevMovementMode, uint8 PreviousCustomMode)
{
	// After the character landed start draining stamina if Movement State is Run or Sprint
	if (PrevMovementMode == MOVE_Falling)
	{
		if (MovementState == EMovementState::Run || MovementState == EMovementState::Sprint)
		{
			StaminaComponent->ServerStartStaminaDrain(MovementState);
		}
	}
	// Stop stamina drain if character jumped
	else if (GetCharacterMovement()->MovementMode == MOVE_Falling)
	{
		StaminaComponent->ServerStopStaminaDrain();
	}
}

bool ABaseCharacter::ServerChangeMovementState_Validate(EMovementState NewMovementState)
{
	if (NewMovementState != MovementState)
	{
		return true;
	}
	return false;
}

void ABaseCharacter::ServerChangeMovementState_Implementation(EMovementState NewMovementState)
{
	if (GetLocalRole() == ROLE_Authority)
	{
		MovementState = NewMovementState;
		switch (NewMovementState)
		{
		case 0:
			// Walk
			MovementScale = 0.25f;	// MaxWalkSpeed = 150.0f
			StaminaComponent->ServerStopStaminaDrain();
			GetCharacterMovement()->JumpZVelocity = 300.0f;
			break;
		case 1:
			// Run
			if (StaminaComponent->CurrentStamina > 0.0f)
			{
				UnCrouch();
				MovementScale = 0.5f;	// MaxWalkSpeed = 300.0f
				GetCharacterMovement()->JumpZVelocity = 360.0f;
			}
			break;
		case 2:
			// Sprint
			if (StaminaComponent->CurrentStamina > 0.0f)
			{
				UnCrouch();
				MovementScale = 1.0f;	// MaxWalkSpeed = 600.0f
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
			
			MovementScale = 0.25f;	// MaxWalkSpeed = 150.0f
			StaminaComponent->ServerStopStaminaDrain();
			GetCharacterMovement()->JumpZVelocity = 0.0f;
			break;
		case 4:
			// Prone
			MovementScale = 0.14f;	// MaxWalkSpeed = 84.0f
			StaminaComponent->ServerStopStaminaDrain();
			GetCharacterMovement()->JumpZVelocity = 0.0f;
			break;
		}
	}
	else
	{
		ServerChangeMovementState(NewMovementState);
	}
}

bool ABaseCharacter::ServerInteractWithWeapon_Validate(ABaseCharacter* Self)
{
	if (Self)
	{
		return true;
	}
	return false;
}

void ABaseCharacter::ServerInteractWithWeapon_Implementation(ABaseCharacter* Self)
{
	// TODO
}

bool ABaseCharacter::ServerInteractWithAmmo_Validate(ABaseCharacter* Self)
{
	if (Self)
	{
		return true;
	}
	return false;
}

void ABaseCharacter::ServerInteractWithAmmo_Implementation(ABaseCharacter* Self)
{
	// TODO
}

bool ABaseCharacter::ServerInteractWithHealth_Validate(ABaseCharacter* Self)
{
	if (Self && Self->GetHealthComponent()->CurrentHealth < Self->GetHealthComponent()->MaxHealth)
	{
		return true;
	}
	return false;
}

void ABaseCharacter::ServerInteractWithHealth_Implementation(ABaseCharacter* Self)
{
	if (GetLocalRole() == ROLE_Authority)
	{
		AHealthPickupActor* HealthPickup = Cast<AHealthPickupActor>(FindPickup(Self));
		if (HealthPickup && HealthPickup->PickupType == EPickupType::Health && Self->GetHealthComponent()->CurrentHealth < Self->GetHealthComponent()->MaxHealth)
		{
			Self->GetHealthComponent()->ServerIncreaseHealth(HealthPickup->IncreaseAmount);
			HealthPickup->ServerUpdatePickupState(HealthPickup, EPickupState::Used);
		}
	}
	else
	{
		ServerInteractWithHealth(Self);
	}
}

APickupActor* ABaseCharacter::FindPickup(ABaseCharacter* Self) const
{
	APickupActor* Pickup = nullptr;
	FHitResult HitResult;
	const FVector End = Self->GetActorLocation() + (Self->GetActorUpVector() * FVector(0.0f, 0.0f, -1.0f) * 100.0f);
	TArray<AActor*> Actors;
	Actors.Add(Self);
	const bool bHit = UKismetSystemLibrary::BoxTraceSingle(GetWorld(), Self->GetActorLocation(), End, FVector(Self->GetCapsuleComponent()->GetScaledCapsuleRadius()),
		FRotator::ZeroRotator, TraceTypeQuery1, false, Actors, EDrawDebugTrace::None, HitResult, true);
	if (bHit)
	{
		Pickup = Cast<APickupActor>(HitResult.GetActor());
	}
	
	return Pickup;
}

void ABaseCharacter::ServerSetHealthLevel_Implementation(float CurrentHealth)
{
	if (GetLocalRole() == ROLE_Authority)
	{
		if (CurrentHealth <= 0.0f)	// TODO - Multicast
		{
			GetCharacterMovement()->DisableMovement();
			GetMesh()->SetCollisionProfileName(FName("Ragdoll"), true);
			GetMesh()->SetSimulatePhysics(true);
		}

		ClientUpdateHealth(CurrentHealth);
	}
}

void ABaseCharacter::ClientUpdateHealth_Implementation(float NewHealth)
{
}

void ABaseCharacter::ServerSetStaminaLevel_Implementation(float CurrentStamina)
{
	if (GetLocalRole() == ROLE_Authority)
	{
		if (CurrentStamina <= 0.0f)
		{
			ServerChangeMovementState(EMovementState::Walk);
		}

		ClientUpdateStamina(CurrentStamina);
	}
}

void ABaseCharacter::ClientUpdateStamina_Implementation(float NewStamina)
{
}