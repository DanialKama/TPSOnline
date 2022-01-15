// All Rights Reserved.

#include "Characters/PlayerCharacter.h"
#include "Actors/PickupActor.h"
#include "Camera/CameraComponent.h"
#include "Components/CapsuleComponent.h"
#include "Components/InputComponent.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "Components/StaminaComponent.h"
#include "Components/WidgetComponent.h"
#include "GameFramework/Controller.h"
#include "GameFramework/SpringArmComponent.h"
#include "Interfaces/WidgetInterface.h"
#include "Kismet/GameplayStatics.h"

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

	Widget = CreateDefaultSubobject<UWidgetComponent>(TEXT("Info Bar"));
	Widget->SetupAttachment(GetRootComponent());
	Widget->SetComponentTickEnabled(false);
	Widget->SetTickMode(ETickMode::Disabled);
	Widget->SetGenerateOverlapEvents(false);
	Widget->CanCharacterStepUpOn = ECB_No;
	Widget->SetWidgetSpace(EWidgetSpace::Screen);
	Widget->InitWidget();

	// Initialize variables
	BaseTurnRate = 45.0f;
	BaseLookUpRate = 45.0f;
	bWidgetInterface = false;
}

void APlayerCharacter::SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent)
{
	// Set up gameplay key bindings
	check(PlayerInputComponent);
	PlayerInputComponent->BindAction("Jump", IE_Pressed, this, &APlayerCharacter::AttemptJump);
	PlayerInputComponent->BindAction("Jump", IE_Released, this, &ACharacter::StopJumping);

	PlayerInputComponent->BindAction("Sprint", IE_Pressed, this, &APlayerCharacter::StartSprint);
	PlayerInputComponent->BindAction("Sprint", IE_Released, this, &APlayerCharacter::StopSprint);

	PlayerInputComponent->BindAction("Interact", IE_Pressed, this, &APlayerCharacter::Interact);

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
	
	UGameplayStatics::GetPlayerCameraManager(GetWorld(), 0)->ViewPitchMax = 50.0f;
	UGameplayStatics::GetPlayerCameraManager(GetWorld(), 0)->ViewPitchMin = -80.0f;
}

void APlayerCharacter::TurnAtRate(float Rate)
{
	// calculate delta for this frame from the rate information
	AddControllerYawInput(Rate * BaseTurnRate * GetWorld()->GetDeltaSeconds());
}

void APlayerCharacter::LookUpAtRate(float Rate)
{
	// calculate delta for this frame from the rate information
	AddControllerPitchInput(Rate * BaseLookUpRate * GetWorld()->GetDeltaSeconds());
}

void APlayerCharacter::MoveForward(float Value)
{
	if (Controller && Value != 0.0f)
	{
		// find out which way is forward
		const FRotator Rotation = Controller->GetControlRotation();
		const FRotator YawRotation(0, Rotation.Yaw, 0);

		// get forward vector
		const FVector Direction = FRotationMatrix(YawRotation).GetUnitAxis(EAxis::X);
		AddMovementInput(Direction, Value * MovementScale);
	}
}

void APlayerCharacter::MoveRight(float Value)
{
	if (Controller && Value != 0.0f)
	{
		// find out which way is right
		const FRotator Rotation = Controller->GetControlRotation();
		const FRotator YawRotation(0, Rotation.Yaw, 0);
	
		// get right vector 
		const FVector Direction = FRotationMatrix(YawRotation).GetUnitAxis(EAxis::Y);
		// add movement in that direction
		AddMovementInput(Direction, Value * MovementScale);
	}
}

void APlayerCharacter::AttemptJump()
{
	if (GetStaminaComponent()->CurrentStamina > 0.0f && !GetCharacterMovement()->IsFalling())
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
	APickupActor* Pickup = FindPickup(this);
	if (Pickup)
	{
		switch (Pickup->PickupType)
		{
		case 0:
			// Weapon
			ServerInteractWithWeapon(this);
			break;
		case 1:
			// Ammo
			ServerInteractWithAmmo(this);
			break;
		case 2:
			// Health, If Current Health is lower than Max Health
			if (GetHealthComponent()->CurrentHealth < GetHealthComponent()->MaxHealth)
			{
				ServerInteractWithHealth(this);
			}
			break;
		}
	}
}

void APlayerCharacter::SetStaminaLevel(float CurrentStamina)
{
	Super::SetStaminaLevel(CurrentStamina);

	if (bWidgetInterface)
	{
		// Set stamina level on widget
		IWidgetInterface::Execute_UpdateStamina(Widget->GetWidget(), CurrentStamina);
	}
	// else if (Widget->GetWidget()->GetClass()->ImplementsInterface(UWidgetInterface::StaticClass())) // TODO - Fix this
	// {
	// 	bWidgetInterface = true;
	// 	IWidgetInterface::Execute_UpdateStamina(Widget->GetWidget(), CurrentStamina);
	// }
	UE_LOG(LogTemp, Warning, TEXT("Result = %s"), IsValid(Widget->GetWidget()) ? TEXT("True") : TEXT("False"));
	// UE_LOG(LogTemp, Warning, TEXT("Widget %s"), *Widget->GetWidget()->GetName());
}