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
	InventoryComponent = CreateDefaultSubobject<UInventoryComponent>(TEXT("Inventory Component"));

	// Initialize variables
	RespawnDelay = 5.0f;
	bDoOnceMoving = true;
	bDoOnceStopped = true;
}

void ABaseCharacter::GetLifetimeReplicatedProps(TArray<FLifetimeProperty> &OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	// Replicate to everyone
	DOREPLIFETIME(ABaseCharacter, MovementState);
	DOREPLIFETIME(ABaseCharacter, MovementScale);
	DOREPLIFETIME(ABaseCharacter, RespawnDelay);
}

void ABaseCharacter::BeginPlay()
{
	Super::BeginPlay();

	if (GetLocalRole() == ROLE_Authority)
	{
		ServerChangeMovementState(EMovementState::Walk);
		HealthComponent->ServerInitialize();
		StaminaComponent->ServerInitialize();
	}
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
				StaminaComponent->ServerStopStaminaDrain(true);
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
	if (GetLocalRole() == ROLE_AutonomousProxy)
	{
		ServerCheckMovementMode(PrevMovementMode);
	}
}

void ABaseCharacter::ServerCheckMovementMode_Implementation(EMovementMode PrevMovementMode)
{
	if (GetLocalRole() == ROLE_Authority)
	{
		// After the character landed start draining stamina if Movement State is Run or Sprint
		if (PrevMovementMode == MOVE_Falling)
		{
			if (MovementState == EMovementState::Run || MovementState == EMovementState::Sprint)
			{
				StaminaComponent->ServerStartStaminaDrain(MovementState);
			}
			else
			{
				StaminaComponent->ServerStopStaminaDrain(true);
			}
		}
		// Stop stamina drain if character jumped
		else if (GetCharacterMovement()->MovementMode == MOVE_Falling)
		{
			StaminaComponent->ServerStopStaminaDrain(false);
		}
	}
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
			StaminaComponent->ServerStopStaminaDrain(true);
			GetCharacterMovement()->JumpZVelocity = 300.0f;
			break;
		case 1:
			// Run
			if (StaminaComponent->CurrentStamina > 0.0f)
			{
				UnCrouch();
				MovementScale = 0.75f;	// MaxWalkSpeed = 300.0f
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
			StaminaComponent->ServerStopStaminaDrain(true);
			GetCharacterMovement()->JumpZVelocity = 0.0f;
			break;
		case 4:
			// Prone
			MovementScale = 0.14f;	// MaxWalkSpeed = 84.0f
			StaminaComponent->ServerStopStaminaDrain(true);
			GetCharacterMovement()->JumpZVelocity = 0.0f;
			break;
		}
	}
}

void ABaseCharacter::ServerInteractWithWeapon_Implementation()
{
	if (GetLocalRole() == ROLE_Authority)
	{
		AWeaponPickupActor* NewWeapon = Cast<AWeaponPickupActor>(FindPickup());
		if (NewWeapon)
		{
			ServerAddWeapon(NewWeapon);
		}
	}
}

void ABaseCharacter::ServerAddWeapon_Implementation(AWeaponPickupActor* NewWeapon)
{
	if (GetLocalRole() == ROLE_Authority && NewWeapon)
	{
		// Update state of the new weapon
		NewWeapon->SetOwner(this);
		NewWeapon->ServerUpdatePickupState(EPickupState::PickedUp);

		const FDetachmentTransformRules DetachmentRules = FDetachmentTransformRules(EDetachmentRule::KeepWorld, EDetachmentRule::KeepWorld, EDetachmentRule::KeepRelative, false);
		const FAttachmentTransformRules AttachmentRules = FAttachmentTransformRules(EAttachmentRule::SnapToTarget, EAttachmentRule::SnapToTarget, EAttachmentRule::KeepWorld, true);
		switch (NewWeapon->WeaponType)
		{
		case 0: case 1:
			// Pistol, SMG - Sidearm weapons
			// Drop the previous sidearm
			if (InventoryComponent->SidearmWeapon)
			{
				InventoryComponent->SidearmWeapon->DetachFromActor(DetachmentRules);
				InventoryComponent->SidearmWeapon->ServerUpdatePickupState(EPickupState::Dropped);

				// Update Current Weapon if sidearm weapon was in the character's hand
				if (InventoryComponent->CurrentWeapon == InventoryComponent->SidearmWeapon && InventoryComponent->CurrentWeaponSlot == EWeaponToDo::Sidearm)
				{
					InventoryComponent->CurrentWeapon = nullptr;
					InventoryComponent->CurrentWeaponSlot = EWeaponToDo::NoWeapon;
				}
			}

			InventoryComponent->SidearmWeapon = NewWeapon;
			
			// If the character's hand is currently empty
			if (InventoryComponent->CurrentWeapon == nullptr && InventoryComponent->CurrentWeaponSlot == EWeaponToDo::NoWeapon)
			{
				// Attach new sidearm weapon to the character's hand
				NewWeapon->AttachToComponent(GetMesh(), AttachmentRules, FName("RightHandSocket"));
				// Set Current Weapon to the new sidearm weapon
				InventoryComponent->CurrentWeapon = NewWeapon;
				InventoryComponent->CurrentWeaponSlot = EWeaponToDo::Primary;
			}
			else if (InventoryComponent->CurrentWeapon != nullptr && InventoryComponent->CurrentWeaponSlot != EWeaponToDo::NoWeapon)
			{
				// Holster the new sidearm weapon if the character's hand is currently not empty
				NewWeapon->AttachToComponent(GetMesh(), AttachmentRules, FName("SidearmWeaponSocket"));
			}
			break;
		case 2: case 3: case 4: case 5: case 6:
			// Rifle, LMG, Shotgun, Sniper, Launcher - Primary and secondary weapons
			// If the character's hand is currently empty
			if (InventoryComponent->CurrentWeapon == nullptr && InventoryComponent->CurrentWeaponSlot == EWeaponToDo::NoWeapon)
			{
				// Add the new weapon as a primary weapon if currently there is no primary weapon
				if (InventoryComponent->PrimaryWeapon == nullptr)
				{
					// Attach the primary weapon to the character's hand
					NewWeapon->AttachToComponent(GetMesh(), AttachmentRules, FName("RightHandSocket"));
					InventoryComponent->PrimaryWeapon = NewWeapon;
					InventoryComponent->CurrentWeapon = NewWeapon;
					InventoryComponent->CurrentWeaponSlot = EWeaponToDo::Primary;
				}
				// Add the new weapon as a secondary weapon if currently there is no secondary weapon
				else if (InventoryComponent->SecondaryWeapon == nullptr)
				{
					// Attach the secondary weapon to the character's hand
					NewWeapon->AttachToComponent(GetMesh(), AttachmentRules, FName("RightHandSocket"));
					InventoryComponent->SecondaryWeapon = NewWeapon;
					InventoryComponent->CurrentWeapon = NewWeapon;
					InventoryComponent->CurrentWeaponSlot = EWeaponToDo::Secondary;
				}
				else
				{
					// Drop the previous secondary weapon and attach new weapon to character's hand
					InventoryComponent->SecondaryWeapon->DetachFromActor(DetachmentRules);
					InventoryComponent->SecondaryWeapon->ServerUpdatePickupState(EPickupState::Dropped);
					NewWeapon->AttachToComponent(GetMesh(), AttachmentRules, FName("RightHandSocket"));
					InventoryComponent->SecondaryWeapon = NewWeapon;
					InventoryComponent->CurrentWeapon = NewWeapon;
					InventoryComponent->CurrentWeaponSlot = EWeaponToDo::Secondary;
				}
			}
			// Replace the new weapon with the current weapon if both primary and secondary weapon slots are not free
			else if (InventoryComponent->CurrentWeapon != nullptr && InventoryComponent->CurrentWeaponSlot != EWeaponToDo::NoWeapon)
			{
				// Add the new weapon as a primary weapon if currently there is no primary weapon
				if (InventoryComponent->PrimaryWeapon == nullptr)
				{
					// Attach the primary weapon to the Primary Weapon Socket
					NewWeapon->AttachToComponent(GetMesh(), AttachmentRules, FName("PrimaryWeaponSocket"));
					InventoryComponent->PrimaryWeapon = NewWeapon;
				}
				// Add the new weapon as a secondary weapon if currently there is no secondary weapon
				else if (InventoryComponent->SecondaryWeapon == nullptr)
				{
					// Attach the secondary weapon to the Secondary Weapon Socket
					NewWeapon->AttachToComponent(GetMesh(), AttachmentRules, FName("SecondaryWeaponSocket"));
					InventoryComponent->SecondaryWeapon = NewWeapon;
				}
				// If character currently holding the primary weapon
				else if (InventoryComponent->CurrentWeapon == InventoryComponent->PrimaryWeapon && InventoryComponent->CurrentWeaponSlot == EWeaponToDo::Primary)
				{
					// Replace the primary weapon and update the current weapon
					InventoryComponent->PrimaryWeapon->DetachFromActor(DetachmentRules);
					InventoryComponent->PrimaryWeapon->ServerUpdatePickupState(EPickupState::Dropped);
					NewWeapon->AttachToComponent(GetMesh(), AttachmentRules, FName("RightHandSocket"));
					InventoryComponent->PrimaryWeapon = NewWeapon;
					InventoryComponent->CurrentWeapon = NewWeapon;
					InventoryComponent->CurrentWeaponSlot = EWeaponToDo::Primary;
				}
				// If character currently holding the secondary weapon
				else if (InventoryComponent->CurrentWeapon == InventoryComponent->SecondaryWeapon && InventoryComponent->CurrentWeaponSlot == EWeaponToDo::Secondary)
				{
					// Replace the secondary weapon and update the current weapon
					InventoryComponent->SecondaryWeapon->DetachFromActor(DetachmentRules);
					InventoryComponent->SecondaryWeapon->ServerUpdatePickupState(EPickupState::Dropped);
					NewWeapon->AttachToComponent(GetMesh(), AttachmentRules, FName("RightHandSocket"));
					InventoryComponent->SecondaryWeapon = NewWeapon;
					InventoryComponent->CurrentWeapon = NewWeapon;
					InventoryComponent->CurrentWeaponSlot = EWeaponToDo::Secondary;
				}
			}
			break;
		}
	}
}

bool ABaseCharacter::ServerInteractWithAmmo_Validate()
{
	return true;	// TODO - only add ammo if there is enough space in inventory
}

void ABaseCharacter::ServerInteractWithAmmo_Implementation()
{
	if (GetLocalRole() == ROLE_Authority)
	{
		// TODO
	}
}

bool ABaseCharacter::ServerInteractWithHealth_Validate()
{
	if (HealthComponent->CurrentHealth < HealthComponent->MaxHealth)
	{
		return true;
	}
	return false;
}

void ABaseCharacter::ServerInteractWithHealth_Implementation()
{
	if (GetLocalRole() == ROLE_Authority)
	{
		AHealthPickupActor* HealthPickup = Cast<AHealthPickupActor>(FindPickup());
		if (HealthPickup && HealthPickup->PickupType == EPickupType::Health && HealthComponent->CurrentHealth < HealthComponent->MaxHealth)
		{
			HealthComponent->ServerIncreaseHealth(HealthPickup->IncreaseAmount);
			HealthPickup->ServerUpdatePickupState(EPickupState::Used);
		}
	}
}

APickupActor* ABaseCharacter::FindPickup() const
{
	APickupActor* Pickup = nullptr;
	FHitResult HitResult;
	const FVector End = GetActorLocation() + (GetActorUpVector() * FVector(0.0f, 0.0f, -1.0f) * 100.0f);
	const TArray<AActor*> Actors;
	const bool bHit = UKismetSystemLibrary::BoxTraceSingle(GetWorld(), GetActorLocation(), End, FVector(GetCapsuleComponent()->GetScaledCapsuleRadius()),
		FRotator::ZeroRotator, TraceTypeQuery3, false, Actors, EDrawDebugTrace::ForDuration, HitResult, true);
	if (bHit)
	{
		Pickup = Cast<APickupActor>(HitResult.GetActor());
	}
	
	return Pickup;
}

void ABaseCharacter::ServerSetHealthLevel_Implementation(ABaseCharacter* ComponentOwner, float CurrentHealth, float MaxHealth)
{
	if (GetLocalRole() == ROLE_Authority)
	{
		// Start restoring health if 0 < Current Health < Max Health and restoring health is not started yet.
		if (CurrentHealth > 0.0f && CurrentHealth < MaxHealth && HealthComponent->bRestoreHealth != true)
		{
			ComponentOwner->HealthComponent->ServerStartRestoreHealth();
		}
		if (CurrentHealth <= 0.0f)
		{
			MulticastDeath();

			FTimerHandle TimerHandle;
			GetWorld()->GetTimerManager().SetTimer(TimerHandle, this, &ABaseCharacter::ServerStartDestroy, RespawnDelay);
		}
		
		ClientUpdateHealth(CurrentHealth / MaxHealth);
	}
}

void ABaseCharacter::ClientUpdateHealth_Implementation(float NewHealth)
{
}

void ABaseCharacter::ServerSetStaminaLevel_Implementation(ABaseCharacter* ComponentOwner, float CurrentStamina, float MaxStamina)
{
	if (GetLocalRole() == ROLE_Authority)
	{
		if (CurrentStamina >= MaxStamina && ComponentOwner->HealthComponent->CurrentHealth < ComponentOwner->HealthComponent->MaxHealth)
		{
			ComponentOwner->HealthComponent->ServerStartRestoreHealth();
		}
		else if (CurrentStamina > 0.0f && CurrentStamina < MaxStamina)
		{
			ComponentOwner->HealthComponent->ServerStopRestoreHealth();
		}
		else if (CurrentStamina <= 0.0f)
		{
			ServerChangeMovementState(EMovementState::Walk);
		}
		
		ClientUpdateStamina(CurrentStamina / MaxStamina);
	}
}

void ABaseCharacter::ClientUpdateStamina_Implementation(float NewStamina)
{
}

void ABaseCharacter::MulticastDeath_Implementation()
{
	GetCharacterMovement()->DisableMovement();
	GetMesh()->SetConstraintProfileForAll(FName("Ragdoll"), true);
	GetMesh()->SetCollisionProfileName(FName("Ragdoll"));
	GetMesh()->SetSimulatePhysics(true);
	GetCapsuleComponent()->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	HealthComponent->DestroyComponent();
	StaminaComponent->DestroyComponent();
}

void ABaseCharacter::ServerStartDestroy_Implementation()
{
	if (GetLocalRole() == ROLE_Authority)
	{
		Destroy();
	}
}