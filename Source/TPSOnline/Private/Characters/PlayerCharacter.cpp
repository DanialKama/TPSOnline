// Copyright 2022 Danial Kamali. All Rights Reserved.

#include "PlayerCharacter.h"

#include "GameFramework/SpringArmComponent.h"
#include "Camera/CameraComponent.h"
#include "Components/TimelineComponent.h"
#include "Components/SlateWrapperTypes.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "Components/InputComponent.h"
#include "Components/HealthComponent.h"
#include "Components/StaminaComponent.h"
#include "Core/CustomPlayerState.h"
#include "Core/DeathmatchGameMode.h"
#include "Actors/PickupActor.h"
#include "Actors/AmmoPickupActor.h"
#include "Actors/WeaponPickupActor.h"
#include "GameFramework/Controller.h"
#include "Interfaces/CharacterAnimationInterface.h"
#include "Kismet/GameplayStatics.h"
#include "Kismet/KismetMathLibrary.h"
#include "Net/UnrealNetwork.h"
#include "UI/PlayerHUD.h"

APlayerCharacter::APlayerCharacter()
{
	GetMesh()->VisibilityBasedAnimTickOption = EVisibilityBasedAnimTickOption::AlwaysTickPoseAndRefreshBones;
	
	// Don't rotate when the controller rotates. Let that just affect the camera.
	bUseControllerRotationPitch = false;
	bUseControllerRotationYaw = false;
	bUseControllerRotationRoll = false;

	// Create a camera boom (pulls in towards the player if there is a collision)
	SpringArm = CreateDefaultSubobject<USpringArmComponent>(TEXT("Camera Boom"));
	SpringArm->SetupAttachment(RootComponent);
	SpringArm->SetComponentTickEnabled(false);
	SpringArm->SocketOffset.Y = 50.0f;
	SpringArm->ProbeSize = 5.0;
	SpringArm->bUsePawnControlRotation = true; // Rotate the arm based on the controller
	SpringArm->bEnableCameraLag = true;

	// Create a follow camera
	Camera = CreateDefaultSubobject<UCameraComponent>(TEXT("Camera"));
	Camera->SetupAttachment(SpringArm, USpringArmComponent::SocketName); // Attach the camera to the end of the boom and let the boom adjust to match the controller orientation
	Camera->SetComponentTickEnabled(false);
	Camera->bUsePawnControlRotation = false; // Camera does not rotate relative to arm

	AimTimeline = CreateDefaultSubobject<UTimelineComponent>(TEXT("Aim Timeline"));
	
	// Initialize variables
	LookUpPitch = 0.0f;
	BaseTurnRate = 45.0f;
	BaseLookUpRate = 45.0f;
	TimeLineDirection = ETimelineDirection::Forward;
	bDoOnceCrouch = true;
	bCharacterAnimationInterface = false;
	CurrentCamera = Camera;
}

void APlayerCharacter::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	// Replicate to everyone
	DOREPLIFETIME(APlayerCharacter, PlayerControllerRef);
	DOREPLIFETIME(APlayerCharacter, LookUpPitch);
}

void APlayerCharacter::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	// Set up gameplay key bindings
	check(PlayerInputComponent);
	PlayerInputComponent->BindAction("Jump", IE_Pressed, this, &APlayerCharacter::AttemptJump);
	PlayerInputComponent->BindAction("Jump", IE_Released, this, &ACharacter::StopJumping);

	PlayerInputComponent->BindAction("Sprint", IE_Pressed, this, &APlayerCharacter::StartSprint);
	PlayerInputComponent->BindAction("Sprint", IE_Released, this, &APlayerCharacter::StopSprint);

	PlayerInputComponent->BindAction("Crouch", IE_Pressed, this, &APlayerCharacter::ToggleCrouch);

	PlayerInputComponent->BindAction("Interact", IE_Pressed, this, &APlayerCharacter::Interact);

	PlayerInputComponent->BindAction("Drop", IE_Pressed, this, &APlayerCharacter::DropCurrentWeapon);

	PlayerInputComponent->BindAction("Reload", IE_Pressed, this, &APlayerCharacter::ReloadWeapon);

	PlayerInputComponent->BindAction("Aim", IE_Pressed, this, &APlayerCharacter::StartAim);
	PlayerInputComponent->BindAction("Aim", IE_Released, this, &APlayerCharacter::StopAim);
	
	PlayerInputComponent->BindAction("FireWeapon", IE_Pressed, this, &APlayerCharacter::StartFireWeapon);
	PlayerInputComponent->BindAction("FireWeapon", IE_Released, this, &APlayerCharacter::StopFireWeapon);
	
	PlayerInputComponent->BindAxis("MoveForward", this, &APlayerCharacter::MoveForward);
	PlayerInputComponent->BindAxis("MoveRight", this, &APlayerCharacter::MoveRight);
	
	// We have 2 versions of the rotation bindings to handle different kinds of devices differently
	// "Turn" handles devices that provide an absolute delta, such as a mouse.
	// "TurnRate" is for devices that we choose to treat as a rate of change, such as an analog joystick
	PlayerInputComponent->BindAxis("Turn", this, &APawn::AddControllerYawInput);
	PlayerInputComponent->BindAxis("TurnRate", this, &APlayerCharacter::TurnAtRate);
	PlayerInputComponent->BindAxis("LookUp", this, &APlayerCharacter::UpdateControllerPitch);
	PlayerInputComponent->BindAxis("LookUpRate", this, &APlayerCharacter::LookUpAtRate);
}

void APlayerCharacter::BeginPlay()
{
	Super::BeginPlay();

	if (GetLocalRole() == ROLE_AutonomousProxy)
	{
		AnimInstance = GetMesh()->GetAnimInstance();
		if (AnimInstance && AnimInstance->GetClass()->ImplementsInterface(UCharacterAnimationInterface::StaticClass()))
		{
			bCharacterAnimationInterface = true;
		}

		UGameplayStatics::GetPlayerCameraManager(GetWorld(), 0)->ViewPitchMin = -80.0f;
		UGameplayStatics::GetPlayerCameraManager(GetWorld(), 0)->ViewPitchMax = 50.0f;

		PlayerHUD = Cast<APlayerHUD>(PlayerControllerRef->GetHUD());
		if (PlayerHUD)
		{
			PlayerHUD->Initialize();
		}

		if (AimFloatCurve)
		{
			FOnTimelineFloat AimTimeLineProgress{};
			AimTimeLineProgress.BindUFunction(this, FName("AimTimeLineUpdate"));
			AimTimeline->AddInterpFloat(AimFloatCurve, AimTimeLineProgress, FName("Alpha"));
			FOnTimelineEvent AimTimelineFinishEvent{};
			AimTimelineFinishEvent.BindUFunction(this, FName("AimTimeLineFinished"));
			AimTimeline->SetTimelineFinishedFunc(AimTimelineFinishEvent);
		}
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
		const FVector NewDirection = FRotationMatrix(YawRotation).GetUnitAxis(EAxis::X);
		AddMovementInput(NewDirection, Value * MovementScale);
	}
}

void APlayerCharacter::MoveRight(float Value)
{
	if (Controller && Value != 0.0f)
	{
		// Zero out pitch and roll, only move on plane, find out which way is right
		const FRotator YawRotation(0, Controller->GetControlRotation().Yaw, 0);
		// Get right vector 
		const FVector NewDirection = FRotationMatrix(YawRotation).GetUnitAxis(EAxis::Y);
		// Add movement in that direction
		AddMovementInput(NewDirection, Value * MovementScale);
	}
}

void APlayerCharacter::UpdateControllerPitch(float Value)
{
	AddControllerPitchInput(Value);
	if (Value != 0.0f)
	{
		const float NewPitch = UKismetMathLibrary::NormalizedDeltaRotator(GetControlRotation(), GetActorRotation()).Pitch;
		ServerUpdateLookUp(NewPitch);
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
	if (Rate != 0.0f)
	{
		const float NewPitch = UKismetMathLibrary::NormalizedDeltaRotator(GetControlRotation(), GetActorRotation()).Pitch;
		ServerUpdateLookUp(NewPitch);
	}
}

void APlayerCharacter::ServerUpdateLookUp_Implementation(float Pitch)
{
	LookUpPitch = Pitch;
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

void APlayerCharacter::ToggleCrouch()
{
	if (bDoOnceCrouch)
	{
		if (MovementState != EMovementState::Crouch)
		{
			ServerChangeMovementState(EMovementState::Crouch);
		}
		else
		{
			ServerChangeMovementState(EMovementState::Walk);
		}

		bDoOnceCrouch = false;
		// Resetting crouch by a delay to stop the player from spamming the crouch.
		FTimerHandle TimerHandle;
		GetWorld()->GetTimerManager().SetTimer(TimerHandle, this, &APlayerCharacter::ResetCrouch, 0.3f);
	}
}

void APlayerCharacter::ResetCrouch()
{
	bDoOnceCrouch = true;
}

void APlayerCharacter::Interact()
{
	if (APickupActor* Pickup = FindPickup())
	{
		switch (Pickup->PickupType)
		{
		case 0:
			// Weapon
			ServerInteractWithWeapon();
			break;
		case 1:
			// Ammo
			{
				if (const AAmmoPickupActor* AmmoPickup = Cast<AAmmoPickupActor>(Pickup))
				{
					switch (AmmoPickup->AmmoType)
					{
					case 0:
						// 5.56
						if (PlayerStateRef->FiveFiveSixAmmo < 120)
						{
							ServerInteractWithAmmo();
						}
						break;
					case 1:
						// 7.62
						if (PlayerStateRef->SevenSixTwoAmmo < 120)
						{
							ServerInteractWithAmmo();
						}
						break;
					case 2:
						// .45
						if (PlayerStateRef->FortyFiveAmmo < 90)
						{
							ServerInteractWithAmmo();
						}
						break;
					case 3:
						// 40 mm HE Grenade
						if (PlayerStateRef->HighExplosive < 90)
						{
							ServerInteractWithAmmo();
						}
						break;
					}
				}
			}
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

void APlayerCharacter::StartAim()
{
	if (CurrentWeapon && CurrentWeaponSlot != EWeaponToDo::NoWeapon)
	{
		ServerUpdateAimState(true);
		
		UGameplayStatics::GetPlayerCameraManager(GetWorld(), 0)->ViewPitchMin = -35.0f;
		UGameplayStatics::GetPlayerCameraManager(GetWorld(), 0)->ViewPitchMax = 55.0f;

		AimTimeline->Play();
		TimeLineDirection = ETimelineDirection::Forward;
	}
}

void APlayerCharacter::StopAim()
{
	ServerUpdateAimState(false);

	AimTimeline->Reverse();
	TimeLineDirection = ETimelineDirection::Backward;

	if (PlayerHUD)
	{
		PlayerHUD->SetCrosshairVisibility(ESlateVisibility::Hidden);
	}
}

void APlayerCharacter::AimTimeLineUpdate(float Value)
{
	if (TimeLineDirection == ETimelineDirection::Forward)
	{
		Camera->SetFieldOfView(FMath::Lerp(Camera->FieldOfView, 50.0f, Value));
		SpringArm->SocketOffset.Y = FMath::Lerp(SpringArm->SocketOffset.Y, 50.0f, Value);
		SpringArm->TargetArmLength = FMath::Lerp(SpringArm->TargetArmLength, 150.0f, Value);
	}
	else
	{
		Camera->SetFieldOfView(FMath::Lerp(90.0f, Camera->FieldOfView, Value));
		SpringArm->SocketOffset.Y = FMath::Lerp(50.0f, SpringArm->SocketOffset.Y, Value);
		SpringArm->TargetArmLength = FMath::Lerp(300.0f, SpringArm->TargetArmLength, Value);
	}
}

void APlayerCharacter::AimTimeLineFinished()
{
	if (TimeLineDirection == ETimelineDirection::Forward)
	{
		if (PlayerHUD)
		{
			PlayerHUD->SetCrosshairVisibility(ESlateVisibility::HitTestInvisible);
		}
	}
	else
	{
		UGameplayStatics::GetPlayerCameraManager(GetWorld(), 0)->ViewPitchMin = -80.0f;
		UGameplayStatics::GetPlayerCameraManager(GetWorld(), 0)->ViewPitchMax = 50.0f;
	}
}

void APlayerCharacter::StartFireWeapon()
{
	if (bCanFireWeapon && CanFireWeapon())
	{
		AddRecoil();

		if (CurrentWeapon->bIsAutomatic)
		{
			GetWorld()->GetTimerManager().SetTimer(RecoilTimer, this, &APlayerCharacter::AddRecoil, CurrentWeapon->TimeBetweenShots, true);
		}

		ServerStartFireWeapon();
	}
}

void APlayerCharacter::StopFireWeapon()
{
	if (CurrentWeapon && CurrentWeaponSlot != EWeaponToDo::NoWeapon)
	{
		GetWorld()->GetTimerManager().ClearTimer(RecoilTimer);
		ServerStopFireWeapon();
	}
}

void APlayerCharacter::AddRecoil()
{
	if (CanFireWeapon())
	{
		if (bIsAiming)
		{
			AddControllerPitchInput(CurrentWeapon->RecoilData.ControllerPitch);
			const float NewPitch = UKismetMathLibrary::NormalizedDeltaRotator(GetControlRotation(), GetActorRotation()).Pitch;
			ServerUpdateLookUp(NewPitch);
		}
		
		PlayerControllerRef->ClientStartCameraShake(CurrentWeapon->Effects.CameraShake);

		if (bCharacterAnimationInterface)
		{
			ICharacterAnimationInterface::Execute_AddRecoil(AnimInstance, CurrentWeapon->RecoilData.RotationIntensity, CurrentWeapon->RecoilData.ControlTime);
		}

		if (PlayerHUD)
		{
			PlayerHUD->AddCrosshairRecoil(CurrentWeapon->RecoilData.CrosshairRecoil, CurrentWeapon->RecoilData.ControlTime);
		}
	}
	else
	{
		GetWorld()->GetTimerManager().ClearTimer(RecoilTimer);
	}
}

void APlayerCharacter::ReloadWeapon()
{
	if (bDoOnceReload && CanReloadWeapon())
	{
		ServerReloadWeapon();
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

void APlayerCharacter::OnRep_CurrentWeapon()
{
	if (GetLocalRole() == ROLE_AutonomousProxy && PlayerHUD)
	{
		if (CurrentWeapon)
		{
			PlayerHUD->SetWeaponInfoVisibility(ESlateVisibility::HitTestInvisible);
			PlayerHUD->UpdateWeaponInfo(CurrentWeapon->WeaponName, FindCurrentAmmo());
			PlayerHUD->UpdateCurrentMagAmmo(CurrentWeapon->CurrentMagazineAmmo);
		}
		else
		{
			PlayerHUD->SetWeaponInfoVisibility(ESlateVisibility::Hidden);
		}
	}
}

int32 APlayerCharacter::FindCurrentAmmo() const
{
	if (PlayerStateRef)
	{
		switch (CurrentWeapon->AmmoType)
		{
		case 0:
			// 5.56 mm
			return PlayerStateRef->FiveFiveSixAmmo;
		case 1:
			// 7.62 mm
			return PlayerStateRef->SevenSixTwoAmmo;
		case 2:
			// .45 ACP
			return PlayerStateRef->FortyFiveAmmo;
		case 3:
			// 40 mm HE Grenade
			return PlayerStateRef->HighExplosive;
		}
	}
	return 0;
}

void APlayerCharacter::ClientUpdateHealth_Implementation(float NewHealth)
{
	if (GetLocalRole() < ROLE_Authority && PlayerHUD)
	{
		PlayerHUD->UpdateHealth(NewHealth);
	}
}

void APlayerCharacter::ClientUpdateStamina_Implementation(float NewStamina)
{
	if (GetLocalRole() < ROLE_Authority && PlayerHUD)
	{
		PlayerHUD->UpdateStamina(NewStamina);
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
		if (ADeathmatchGameMode* GameMode = Cast<ADeathmatchGameMode>(GetWorld()->GetAuthGameMode()))
		{
			GameMode->ServerStartRespawn(RespawnControllerRef);
		}
	}
}
