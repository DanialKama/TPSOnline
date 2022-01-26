// All Rights Reserved.

#include "Characters/PlayerCharacter.h"
#include "Actors/PickupActor.h"
#include "Camera/CameraComponent.h"
#include "Components/CapsuleComponent.h"
#include "Components/InputComponent.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "Components/HealthComponent.h"
#include "Components/StaminaComponent.h"
#include "Core/CustomPlayerController.h"
#include "Core/CustomPlayerState.h"
#include "Core/DeathmatchGameMode.h"
#include "GameFramework/Controller.h"
#include "GameFramework/SpringArmComponent.h"
#include "Kismet/GameplayStatics.h"
#include "Net/UnrealNetwork.h"
#include "UI/PlayerHUD.h"

APlayerCharacter::APlayerCharacter()
{
	GetCapsuleComponent()->InitCapsuleSize(34.0f, 90.0f);

	GetMesh()->SetRelativeRotation(FRotator(0.0f, -90.0f, 0.0f));
	
	// Don't rotate when the controller rotates. Let that just affect the camera.
	bUseControllerRotationPitch = false;
	bUseControllerRotationYaw = false;
	bUseControllerRotationRoll = false;

	// Configure character movement
	GetCharacterMovement()->bOrientRotationToMovement = true; // Character moves in the direction of input...	
	GetCharacterMovement()->RotationRate = FRotator(0.0f, 540.0f, 0.0f); // ...at this rotation rate
	GetCharacterMovement()->MaxWalkSpeed = 600.0f;
	GetCharacterMovement()->JumpZVelocity = 300.0f;
	GetCharacterMovement()->AirControl = 0.2f;
	GetCharacterMovement()->bCanWalkOffLedgesWhenCrouching = true;

	// Create a camera boom (pulls in towards the player if there is a collision)
	SpringArm = CreateDefaultSubobject<USpringArmComponent>(TEXT("Camera Boom"));
	SpringArm->SetupAttachment(RootComponent);
	SpringArm->SetComponentTickEnabled(false);
	SpringArm->bUsePawnControlRotation = true; // Rotate the arm based on the controller
	SpringArm->bEnableCameraLag = true;

	// Create a follow camera
	Camera = CreateDefaultSubobject<UCameraComponent>(TEXT("Camera"));
	Camera->SetupAttachment(SpringArm, USpringArmComponent::SocketName); // Attach the camera to the end of the boom and let the boom adjust to match the controller orientation
	Camera->SetComponentTickEnabled(false);
	Camera->bUsePawnControlRotation = false; // Camera does not rotate relative to arm

	// Initialize variables
	BaseTurnRate = 45.0f;
	BaseLookUpRate = 45.0f;
}

void APlayerCharacter::GetLifetimeReplicatedProps(TArray<FLifetimeProperty> &OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	// Replicate to everyone
	DOREPLIFETIME(APlayerCharacter, PlayerControllerRef);
}

void APlayerCharacter::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	// Set up gameplay key bindings
	check(PlayerInputComponent);
	PlayerInputComponent->BindAction("Jump", IE_Pressed, this, &APlayerCharacter::AttemptJump);
	PlayerInputComponent->BindAction("Jump", IE_Released, this, &ACharacter::StopJumping);

	PlayerInputComponent->BindAction("Sprint", IE_Pressed, this, &APlayerCharacter::StartSprint);
	PlayerInputComponent->BindAction("Sprint", IE_Released, this, &APlayerCharacter::StopSprint);

	PlayerInputComponent->BindAction("Interact", IE_Pressed, this, &APlayerCharacter::Interact);

	PlayerInputComponent->BindAction("Drop", IE_Pressed, this, &APlayerCharacter::DropCurrentWeapon);

	PlayerInputComponent->BindAxis("MoveForward", this, &APlayerCharacter::MoveForward);
	PlayerInputComponent->BindAxis("MoveRight", this, &APlayerCharacter::MoveRight);
	
	// We have 2 versions of the rotation bindings to handle different kinds of devices differently
	// "Turn" handles devices that provide an absolute delta, such as a mouse.
	// "TurnRate" is for devices that we choose to treat as a rate of change, such as an analog joystick
	PlayerInputComponent->BindAxis("Turn", this, &APawn::AddControllerYawInput);
	PlayerInputComponent->BindAxis("TurnRate", this, &APlayerCharacter::TurnAtRate);
	PlayerInputComponent->BindAxis("LookUp", this, &APawn::AddControllerPitchInput);
	PlayerInputComponent->BindAxis("LookUpRate", this, &APlayerCharacter::LookUpAtRate);
}

void APlayerCharacter::BeginPlay()
{
	Super::BeginPlay();

	if (GetLocalRole() < ROLE_Authority)
	{
		UGameplayStatics::GetPlayerCameraManager(GetWorld(), 0)->ViewPitchMax = 50.0f;
		UGameplayStatics::GetPlayerCameraManager(GetWorld(), 0)->ViewPitchMin = -80.0f;
	}
}

void APlayerCharacter::PossessedBy(AController* NewController)
{
	Super::PossessedBy(NewController);
	
	PlayerControllerRef = Cast<APlayerController>(NewController);
}

void APlayerCharacter::MoveForward(float Value)
{
	if (Controller && Value != 0.0f)
	{
		// Zero out pitch and roll, only move on plane, find out which way is forward
		const FRotator YawRotation(0, Controller->GetControlRotation().Yaw, 0);
		// Get forward vector
		const FVector Direction = FRotationMatrix(YawRotation).GetUnitAxis(EAxis::X);
		AddMovementInput(Direction, Value * MovementScale);
	}
}

void APlayerCharacter::MoveRight(float Value)
{
	if (Controller && Value != 0.0f)
	{
		// Zero out pitch and roll, only move on plane, find out which way is right
		const FRotator YawRotation(0, Controller->GetControlRotation().Yaw, 0);
		// Get right vector 
		const FVector Direction = FRotationMatrix(YawRotation).GetUnitAxis(EAxis::Y);
		// Add movement in that direction
		AddMovementInput(Direction, Value * MovementScale);
	}
}

void APlayerCharacter::TurnAtRate(float Rate)
{
	// Calculate delta for this frame from the rate information
	AddControllerYawInput(Rate * BaseTurnRate * GetWorld()->GetDeltaSeconds());
}

void APlayerCharacter::LookUpAtRate(float Rate)
{
	// Calculate delta for this frame from the rate information
	AddControllerPitchInput(Rate * BaseLookUpRate * GetWorld()->GetDeltaSeconds());
}

void APlayerCharacter::AttemptJump()
{
	if (GetStaminaComponent()->CurrentStamina > 0.0f && GetCharacterMovement()->IsFalling() == false)
	{
		GetStaminaComponent()->ServerJumpDrainStamina();
		Jump();
	}
}

void APlayerCharacter::StartSprint()
{
	if (bDoOnceStopped)
	{
		GetStaminaComponent()->ServerStartStaminaDrain(EMovementState::Sprint);
	}

	ServerChangeMovementState(EMovementState::Sprint);
}

void APlayerCharacter::StopSprint()
{
	ServerChangeMovementState(EMovementState::Walk);
}

void APlayerCharacter::Interact()
{
	APickupActor* Pickup = FindPickup();
	if (Pickup)
	{
		switch (Pickup->PickupType)
		{
		case 0:
			// Weapon
			ServerInteractWithWeapon();
			break;
		case 1:
			// Ammo
			ServerInteractWithAmmo();
			break;
		case 2:
			// Health, If Current Health is lower than Max Health
			if (GetHealthComponent()->CurrentHealth < GetHealthComponent()->MaxHealth)
			{
				ServerInteractWithHealth();
			}
			break;
		}
	}
}

void APlayerCharacter::DropCurrentWeapon()
{
	if (CurrentWeapon && CurrentWeaponSlot != EWeaponToDo::NoWeapon)
	{
		switch (CurrentWeaponSlot)
		{
		case 0:
			// No Weapon = Nothing to drop
			break;
		case 1:
			// Primary Weapon
			ServerDropWeapon(EWeaponToDo::Primary);
			break;
		case 2:
			// Secondary Weapon
			ServerDropWeapon(EWeaponToDo::Secondary);
			break;
		case 3:
			// Sidearm Weapon
			ServerDropWeapon(EWeaponToDo::Sidearm);
			break;
		}
	}
}

void APlayerCharacter::ClientUpdateHealth_Implementation(float NewHealth)
{
	if (GetLocalRole() < ROLE_Authority)
	{
		if (PlayerHUD)
		{
			PlayerHUD->UpdateHealth(NewHealth);
		}
		else
		{
			PlayerHUD = Cast<APlayerHUD>(PlayerControllerRef->GetHUD());
			if (PlayerHUD)
			{
				PlayerHUD->Initialize();
				PlayerHUD->UpdateHealth(NewHealth);
			}
		}
	}
}

void APlayerCharacter::ClientUpdateStamina_Implementation(float NewStamina)
{
	if (GetLocalRole() < ROLE_Authority)
	{
		if (PlayerHUD)
		{
			PlayerHUD->UpdateStamina(NewStamina);
		}
		else
		{
			PlayerHUD = Cast<APlayerHUD>(PlayerControllerRef->GetHUD());
			if (PlayerHUD)
			{
				PlayerHUD->Initialize();
				PlayerHUD->UpdateStamina(NewStamina);
			}
		}
	}
}

void APlayerCharacter::Destroyed()
{
	if (GetLocalRole() == ROLE_Authority)
	{
		// In case of player leave the session without getting killed
		if (bDoOnceDeath && PlayerStateRef)
		{
			bDoOnceDeath = false;
			PlayerStateRef->ServerPlayerDied();
		}

		// Get player controller reference before destroying the player
		AController* RespawnControllerRef = GetController();
	
		Super::Destroyed();

		// Get the World and GameMode in the world to invoke its restart player function.
		ADeathmatchGameMode* GameMode = Cast<ADeathmatchGameMode>(GetWorld()->GetAuthGameMode());
		if (GameMode)
		{
			GameMode->ServerStartRespawn(RespawnControllerRef);
		}
	}
}