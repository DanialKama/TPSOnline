// Copyright 2022 Danial Kamali. All Rights Reserved.

#include "Characters/BaseCharacter.h"
#include "Actors/AmmoPickupActor.h"
#include "Components/CapsuleComponent.h"
#include "Camera/CameraComponent.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "Core/CustomPlayerState.h"
#include "Actors/PickupActor.h"
#include "Actors/HealthPickupActor.h"
#include "ACtors/WeaponPickupActor.h"
#include "Actors/ProjectileActor.h"
#include "Components/HealthComponent.h"
#include "Components/StaminaComponent.h"
#include "Kismet/KismetSystemLibrary.h"
#include "Net/UnrealNetwork.h"

ABaseCharacter::ABaseCharacter()
{
	PrimaryActorTick.bCanEverTick = true;

	GetCapsuleComponent()->InitCapsuleSize(34.0f, 90.0f);
	GetMesh()->SetRelativeRotation(FRotator(0.0f, -90.0f, 0.0f));

	// Configure character movement
	GetCharacterMovement()->bOrientRotationToMovement = true; // Character moves in the direction of input...	
	GetCharacterMovement()->RotationRate = FRotator(0.0f, 540.0f, 0.0f); // ...at this rotation rate
	GetCharacterMovement()->MaxWalkSpeed = 600.0f;
	GetCharacterMovement()->MaxWalkSpeedCrouched = 600.0f;
	GetCharacterMovement()->JumpZVelocity = 300.0f;
	GetCharacterMovement()->SetCrouchedHalfHeight(60.0f);
	GetCharacterMovement()->AirControl = 0.2f;
	GetCharacterMovement()->bCanWalkOffLedgesWhenCrouching = true;
	
	HealthComponent = CreateDefaultSubobject<UHealthComponent>(TEXT("Health Component"));
	StaminaComponent = CreateDefaultSubobject<UStaminaComponent>(TEXT("Stamina Component"));
	
	// Initialize variables
	CurrentWeapon = nullptr;
	PlayerStateRef = nullptr;
	CurrentWeaponSlot = EWeaponToDo::NoWeapon;
	RespawnDelay = 5.0f;
	bDoOnceMoving = bDoOnceStopped = true;
	bDoOnceDeath = true;
	bIsAiming = bIsArmed = false;
	bCanFireWeapon = true;
	bDoOnceReload = true;
}

void ABaseCharacter::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	// Replicate to everyone
	DOREPLIFETIME(ABaseCharacter, PlayerStateRef);
	DOREPLIFETIME(ABaseCharacter, MovementState);
	DOREPLIFETIME(ABaseCharacter, MovementScale);
	DOREPLIFETIME(ABaseCharacter, bIsArmed);
	DOREPLIFETIME(ABaseCharacter, bIsAiming);
	DOREPLIFETIME(ABaseCharacter, bDoOnceReload);
	DOREPLIFETIME(ABaseCharacter, bCanFireWeapon);
	DOREPLIFETIME(ABaseCharacter, CurrentWeapon);
	DOREPLIFETIME(ABaseCharacter, CurrentWeaponSlot);
	DOREPLIFETIME(ABaseCharacter, bDoOnceDeath);
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

void ABaseCharacter::PossessedBy(AController* NewController)
{
	Super::PossessedBy(NewController);
	
	PlayerStateRef = Cast<ACustomPlayerState>(NewController->PlayerState);
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

void ABaseCharacter::ServerChangeMovementState_Implementation(EMovementState NewMovementState)
{
	MovementState = NewMovementState;
	OnRep_MovementState();
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
		if (StaminaComponent->CurrentStamina > 0.0f && bIsAiming == false)
		{
			UnCrouch();
			MovementScale = 0.75f;	// MaxWalkSpeed = 300.0f
			GetCharacterMovement()->JumpZVelocity = 360.0f;
		}
		break;
	case 2:
		// Sprint
		if (StaminaComponent->CurrentStamina > 0.0f && bIsAiming == false)
		{
			UnCrouch();
			MovementScale = 1.0f;	// MaxWalkSpeed = 600.0f
			GetCharacterMovement()->JumpZVelocity = 420.0f;
		}
		break;
	case 3:
		// Crouch
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

void ABaseCharacter::OnRep_MovementState()
{
	if (MovementState != EMovementState::Crouch && GetCharacterMovement()->IsCrouching())
	{
		UnCrouch();
	}
	else if (MovementState == EMovementState::Crouch)
	{
		Crouch();
	}
}

void ABaseCharacter::ServerInteractWithWeapon_Implementation()
{
	AWeaponPickupActor* NewWeapon = Cast<AWeaponPickupActor>(FindPickup());
	if (NewWeapon)
	{
		ServerAddWeapon(NewWeapon);
	}
}

bool ABaseCharacter::ServerAddWeapon_Validate(AWeaponPickupActor* NewWeapon)
{
	if (NewWeapon)
	{
		return true;
	}
	return false;
}

void ABaseCharacter::ServerAddWeapon_Implementation(AWeaponPickupActor* NewWeapon)
{
	// Update state of the new weapon
	NewWeapon->SetOwner(this);
	NewWeapon->PickupState = EPickupState::PickedUp;
	NewWeapon->OnRep_PickupState();
		
	const FAttachmentTransformRules AttachmentRules = FAttachmentTransformRules(EAttachmentRule::SnapToTarget, EAttachmentRule::SnapToTarget, EAttachmentRule::KeepWorld, true);
	switch (NewWeapon->WeaponType)
	{
	case 0: case 1:
		// Pistol, SMG = Sidearm weapons
		// Drop the previous sidearm
		ServerDropWeapon(EWeaponToDo::Sidearm);
			
		// If the character's hand is currently empty
		if (CurrentWeapon == nullptr && CurrentWeaponSlot == EWeaponToDo::NoWeapon)
		{
			// Set Current Weapon to the new sidearm weapon
			NewWeapon->AttachToComponent(GetMesh(), AttachmentRules, FName("RightHandSocket"));
			ServerUpdateCurrentWeapon(NewWeapon, EWeaponToDo::Sidearm);
		}
		else if (CurrentWeapon != nullptr && CurrentWeaponSlot != EWeaponToDo::NoWeapon)
		{
			// Holster the new sidearm weapon if the character's hand is currently not empty
			NewWeapon->AttachToComponent(GetMesh(), AttachmentRules, FName("SidearmWeaponSocket"));
		}
		break;
	case 2: case 3: case 4: case 5: case 6:
		// Rifle, LMG, Shotgun, Sniper, Launcher = Primary and secondary weapons
		// If the character's hand is currently empty
		if (CurrentWeapon == nullptr && CurrentWeaponSlot == EWeaponToDo::NoWeapon)
		{
			// Add the new weapon as a primary weapon if currently there is no primary weapon
			if (PlayerStateRef->PrimaryWeapon == nullptr)
			{
				// Attach the primary weapon to the character's hand
				NewWeapon->AttachToComponent(GetMesh(), AttachmentRules, FName("RightHandSocket"));
				ServerUpdateCurrentWeapon(NewWeapon, EWeaponToDo::Primary);
			}
			// Add the new weapon as a secondary weapon if currently there is no secondary weapon
			else if (PlayerStateRef->SecondaryWeapon == nullptr)
			{
				// Attach the secondary weapon to the character's hand
				NewWeapon->AttachToComponent(GetMesh(), AttachmentRules, FName("RightHandSocket"));
				ServerUpdateCurrentWeapon(NewWeapon, EWeaponToDo::Secondary);
			}
			else
			{
				// Drop the previous secondary weapon and attach new weapon to character's hand
				ServerDropWeapon(EWeaponToDo::Secondary);
				NewWeapon->AttachToComponent(GetMesh(), AttachmentRules, FName("RightHandSocket"));
				ServerUpdateCurrentWeapon(NewWeapon, EWeaponToDo::Secondary);
			}
		}
		// Replace the new weapon with the current weapon if both primary and secondary weapon slots are not free
		else if (CurrentWeapon != nullptr && CurrentWeaponSlot != EWeaponToDo::NoWeapon)
		{
			// Add the new weapon as a primary weapon if currently there is no primary weapon
			if (PlayerStateRef->PrimaryWeapon == nullptr)
			{
				// Attach the primary weapon to its socket
				NewWeapon->AttachToComponent(GetMesh(), AttachmentRules, FName("PrimaryWeaponSocket"));
				PlayerStateRef->PrimaryWeapon = NewWeapon;
			}
			// Add the new weapon as a secondary weapon if currently there is no secondary weapon
			else if (PlayerStateRef->SecondaryWeapon == nullptr)
			{
				// Attach the secondary weapon to its socket
				NewWeapon->AttachToComponent(GetMesh(), AttachmentRules, FName("SecondaryWeaponSocket"));
				PlayerStateRef->SecondaryWeapon = NewWeapon;
			}
			// If character currently holding the primary weapon
			else if (CurrentWeapon == PlayerStateRef->PrimaryWeapon && CurrentWeaponSlot == EWeaponToDo::Primary)
			{
				// Replace the primary weapon
				ServerDropWeapon(EWeaponToDo::Primary);
				NewWeapon->AttachToComponent(GetMesh(), AttachmentRules, FName("RightHandSocket"));
				ServerUpdateCurrentWeapon(NewWeapon, EWeaponToDo::Primary);
			}
			// If character currently holding the secondary weapon
			else if (CurrentWeapon == PlayerStateRef->SecondaryWeapon && CurrentWeaponSlot == EWeaponToDo::Secondary)
			{
				// Replace the secondary weapon
				ServerDropWeapon(EWeaponToDo::Secondary);
				NewWeapon->AttachToComponent(GetMesh(), AttachmentRules, FName("RightHandSocket"));
				ServerUpdateCurrentWeapon(NewWeapon, EWeaponToDo::Secondary);
			}
		}
		break;
	}
}

void ABaseCharacter::ServerUpdateCurrentWeapon_Implementation(AWeaponPickupActor* NewWeapon, EWeaponToDo WeaponToEquip)
{
	CurrentWeapon = NewWeapon;
	switch (WeaponToEquip)
	{
	case 0:
		// No Weapon = Nothing to equip
		bIsArmed = false;
		CurrentWeaponSlot = EWeaponToDo::NoWeapon;
		break;
	case 1:
		// Primary weapon
		bIsArmed = true;
		PlayerStateRef->PrimaryWeapon = NewWeapon;
		CurrentWeaponSlot = EWeaponToDo::Primary;
		break;
	case 2:
		// Secondary weapon
		bIsArmed = true;
		PlayerStateRef->SecondaryWeapon = NewWeapon;
		CurrentWeaponSlot = EWeaponToDo::Secondary;
		break;
	case 3:
		// Sidearm weapon
		bIsArmed = true;
		PlayerStateRef->SidearmWeapon = NewWeapon;
		CurrentWeaponSlot = EWeaponToDo::Sidearm;
		break;
	}
}

bool ABaseCharacter::ServerDropWeapon_Validate(EWeaponToDo WeaponToDrop)
{
	if (WeaponToDrop != EWeaponToDo::NoWeapon)
	{
		return true;
	}
	return false;
}

void ABaseCharacter::ServerDropWeapon_Implementation(EWeaponToDo WeaponToDrop)
{
	AWeaponPickupActor* DroppedWeapon = nullptr;
	const FDetachmentTransformRules DetachmentRules = FDetachmentTransformRules(EDetachmentRule::KeepWorld, EDetachmentRule::KeepWorld, EDetachmentRule::KeepRelative, false);
	switch (WeaponToDrop)
	{
	case 0:
		// No Weapon - Nothing to drop
		break;
	case 1:
		// Primary weapon
		if (PlayerStateRef->PrimaryWeapon)
		{
			PlayerStateRef->PrimaryWeapon->DetachFromActor(DetachmentRules);
			PlayerStateRef->PrimaryWeapon->PickupState = EPickupState::Dropped;
			PlayerStateRef->PrimaryWeapon->OnRep_PickupState();
			DroppedWeapon = PlayerStateRef->PrimaryWeapon;
			PlayerStateRef->PrimaryWeapon = nullptr;
		}
		break;
	case 2:
		// Secondary weapon
		if (PlayerStateRef->SecondaryWeapon)
		{
			PlayerStateRef->SecondaryWeapon->DetachFromActor(DetachmentRules);
			PlayerStateRef->SecondaryWeapon->PickupState = EPickupState::Dropped;
			PlayerStateRef->SecondaryWeapon->OnRep_PickupState();
			DroppedWeapon = PlayerStateRef->SecondaryWeapon;
			PlayerStateRef->SecondaryWeapon = nullptr;
		}
		break;
	case 3:
		// Sidearm weapon
		if (PlayerStateRef->SidearmWeapon)
		{
			PlayerStateRef->SidearmWeapon->DetachFromActor(DetachmentRules);
			PlayerStateRef->SidearmWeapon->PickupState = EPickupState::Dropped;
			PlayerStateRef->SidearmWeapon->OnRep_PickupState();
			DroppedWeapon = PlayerStateRef->SidearmWeapon;
			PlayerStateRef->SidearmWeapon = nullptr;
		}
		break;
	}

	// If the dropped weapon was the current weapon, update the current weapon
	if (DroppedWeapon == CurrentWeapon && WeaponToDrop == CurrentWeaponSlot)
	{
		ServerUpdateCurrentWeapon(nullptr, EWeaponToDo::NoWeapon);
	}
}

void ABaseCharacter::OnRep_CurrentWeapon()
{
	// Override in the player character class
}

void ABaseCharacter::ServerUpdateAimState_Implementation(bool bAim)
{
	bIsAiming = bAim;
	OnRep_IsAiming();
	if (bIsAiming)
	{
		ServerChangeMovementState(EMovementState::Walk);
	}
}

void ABaseCharacter::OnRep_IsAiming() const
{
	if (bIsAiming)
	{
		GetCharacterMovement()->bUseControllerDesiredRotation = true;
		GetCharacterMovement()->bOrientRotationToMovement = false;
	}
	else
	{
		GetCharacterMovement()->bUseControllerDesiredRotation = false;
		GetCharacterMovement()->bOrientRotationToMovement = true;
	}
}

bool ABaseCharacter::ServerStartFireWeapon_Validate()
{
	if (bCanFireWeapon && CanFireWeapon())
	{
		return true;
	}
	return false;
}

void ABaseCharacter::ServerStartFireWeapon_Implementation()
{
	ServerFireWeapon();
	if (CurrentWeapon->bIsAutomatic)
	{
		GetWorld()->GetTimerManager().SetTimer(FireWeaponTimer, this, &ABaseCharacter::ServerFireWeapon, CurrentWeapon->TimeBetweenShots, true);
	}

	bCanFireWeapon = false;
	GetWorld()->GetTimerManager().SetTimer(ResetFireWeaponTimer, this, &ABaseCharacter::ServerResetFireWeapon, CurrentWeapon->TimeBetweenShots);
}

bool ABaseCharacter::ServerStopFireWeapon_Validate()
{
	if (CurrentWeapon && CurrentWeaponSlot != EWeaponToDo::NoWeapon)
	{
		return true;
	}
	return false;
}

void ABaseCharacter::ServerStopFireWeapon_Implementation()
{
	GetWorld()->GetTimerManager().ClearTimer(FireWeaponTimer);
}

bool ABaseCharacter::ServerFireWeapon_Validate()
{
	// Checking if there is any ammo for this weapon, and if not, stop firing the weapon
	if (CanFireWeapon())
	{
		return true;
	}
	GetWorld()->GetTimerManager().ClearTimer(FireWeaponTimer);
	return false;
}

void ABaseCharacter::ServerFireWeapon_Implementation()
{
	CurrentWeapon->CurrentMagazineAmmo = --CurrentWeapon->CurrentMagazineAmmo;
	CurrentWeapon->ServerSpawnProjectile(bIsAiming, CurrentCamera->GetComponentTransform());
}

bool ABaseCharacter::CanFireWeapon() const
{
	if (CurrentWeapon && CurrentWeaponSlot != EWeaponToDo::NoWeapon && CurrentWeapon->CurrentMagazineAmmo > 0)
	{
		return true;
	}
	return false;
}

void ABaseCharacter::ServerResetFireWeapon_Implementation()
{
	bCanFireWeapon = true;
}

bool ABaseCharacter::ServerReloadWeapon_Validate()
{
	if (bDoOnceReload && CanReloadWeapon())
	{
		return true;
	}
	return false;
}

void ABaseCharacter::ServerReloadWeapon_Implementation()
{
	bDoOnceReload = false;
}

bool ABaseCharacter::CanReloadWeapon() const
{
	if (CurrentWeapon && CurrentWeaponSlot != EWeaponToDo::NoWeapon && CurrentWeapon->CurrentMagazineAmmo < CurrentWeapon->MagazineSize)
	{
		switch (CurrentWeapon->AmmoType)
		{
		case 0:
			// 5.56 mm
			if (PlayerStateRef->FiveFiveSixAmmo > 0)
			{
				return true;
			}
			return false;
		case 1:
			// 7.62 mm
			if (PlayerStateRef->SevenSixTwoAmmo > 0)
			{
				return true;
			}
			return false;
		case 2:
			// .45 ACP
			if (PlayerStateRef->FortyFiveAmmo > 0)
			{
				return true;
			}
			return false;
		case 3:
			// 40 mm HE Grenade
			if (PlayerStateRef->HighExplosive > 0)
			{
				return true;
			}
			return false;
		}
	}
	return false;
}

void ABaseCharacter::ServerInteractWithAmmo_Implementation()
{
	AAmmoPickupActor* AmmoPickup = Cast<AAmmoPickupActor>(FindPickup());
	if (AmmoPickup)
	{
		switch (AmmoPickup->AmmoType)
		{
		case 0:
			// 5.56 mm
			if (PlayerStateRef->FiveFiveSixAmmo < 120)
			{
				const int32 AddedAmount = FMath::Clamp(AmmoPickup->Amount, 0, 120 - PlayerStateRef->FiveFiveSixAmmo);
				PlayerStateRef->FiveFiveSixAmmo += AddedAmount;
				AmmoPickup->Amount -= AddedAmount;
			}
			break;
		case 1:
			// 7.62 mm
			if (PlayerStateRef->SevenSixTwoAmmo < 120)
			{
				const int32 AddedAmount = FMath::Clamp(AmmoPickup->Amount, 0, 120 - PlayerStateRef->SevenSixTwoAmmo);
				PlayerStateRef->SevenSixTwoAmmo += AddedAmount;
				AmmoPickup->Amount -= AddedAmount;
			}
			break;
		case 2:
			// .45 ACP
			if (PlayerStateRef->FortyFiveAmmo < 90)
			{
				const int32 AddedAmount = FMath::Clamp(AmmoPickup->Amount, 0, 120 - PlayerStateRef->FortyFiveAmmo);
				PlayerStateRef->FortyFiveAmmo += AddedAmount;
				AmmoPickup->Amount -= AddedAmount;
			}
			break;
		case 3:
			// 40 mm HE Grenade
			if (PlayerStateRef->HighExplosive < 90)
			{
				const int32 AddedAmount = FMath::Clamp(AmmoPickup->Amount, 0, 120 - PlayerStateRef->HighExplosive);
				PlayerStateRef->HighExplosive += AddedAmount;
				AmmoPickup->Amount -= AddedAmount;
			}
			break;
		}
		
		AmmoPickup->PickupState = EPickupState::Used;
		AmmoPickup->OnRep_PickupState();
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
	AHealthPickupActor* HealthPickup = Cast<AHealthPickupActor>(FindPickup());
	if (HealthPickup && HealthPickup->PickupType == EPickupType::Health && HealthComponent->CurrentHealth < HealthComponent->MaxHealth)
	{
		HealthComponent->ServerIncreaseHealth(HealthPickup->IncreaseAmount);
		HealthPickup->PickupState = EPickupState::Used;
		HealthPickup->OnRep_PickupState();
	}
}

APickupActor* ABaseCharacter::FindPickup() const
{
	APickupActor* Pickup = nullptr;
	FHitResult HitResult;
	const FVector End = GetActorLocation() + (GetActorUpVector() * FVector(0.0f, 0.0f, -1.0f) * 100.0f);
	const TArray<AActor*> Actors;
	const bool bHit = UKismetSystemLibrary::BoxTraceSingle(GetWorld(), GetActorLocation(), End, FVector(GetCapsuleComponent()->GetScaledCapsuleRadius()),
		FRotator::ZeroRotator, TraceTypeQuery3, false, Actors, EDrawDebugTrace::None, HitResult, true);
	if (bHit)
	{
		Pickup = Cast<APickupActor>(HitResult.GetActor());
	}
	
	return Pickup;
}

void ABaseCharacter::ServerSetHealthLevel_Implementation(float CurrentHealth, float MaxHealth)
{
	// Start restoring health if 0 < Current Health < Max Health and restoring health is not started yet.
	if (CurrentHealth > 0.0f && CurrentHealth < MaxHealth && HealthComponent->bRestoreHealth != true)
	{
		HealthComponent->ServerStartRestoreHealth();
	}
	if (CurrentHealth <= 0.0f && bDoOnceDeath)
	{
		bDoOnceDeath = false;
		MulticastDeath();

		FTimerHandle TimerHandle;
		GetWorld()->GetTimerManager().SetTimer(TimerHandle, this, &ABaseCharacter::ServerStartDestroy, RespawnDelay);
	}
		
	ClientUpdateHealth(CurrentHealth / MaxHealth);
}

void ABaseCharacter::ClientUpdateHealth_Implementation(float NewHealth)
{
	// Override in the player character class
}

void ABaseCharacter::ServerSetStaminaLevel_Implementation(float CurrentStamina, float MaxStamina)
{
	if (CurrentStamina >= MaxStamina && HealthComponent->CurrentHealth < HealthComponent->MaxHealth)
	{
		HealthComponent->ServerStartRestoreHealth();
	}
	else if (CurrentStamina > 0.0f && CurrentStamina < MaxStamina)
	{
		HealthComponent->ServerStopRestoreHealth();
	}
	else if (CurrentStamina <= 0.0f)
	{
		ServerChangeMovementState(EMovementState::Walk);
	}
		
	ClientUpdateStamina(CurrentStamina / MaxStamina);
}

void ABaseCharacter::ClientUpdateStamina_Implementation(float NewStamina)
{
	// Override in the player character class
}

void ABaseCharacter::MulticastDeath_Implementation()
{
	if (GetLocalRole() == ROLE_Authority)
	{
		PlayerStateRef->ServerPlayerDied();
	}

	GetMesh()->bPauseAnims = true;
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
	Destroy();
}