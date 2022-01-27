// All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "Actors/PickupActor.h"
#include "WeaponPickupActor.generated.h"

USTRUCT(BlueprintType)
struct FEffects
{
	GENERATED_BODY()

	UPROPERTY(EditDefaultsOnly)
	FVector MuzzleFlashScale;

	UPROPERTY(EditDefaultsOnly)
	UParticleSystem* MuzzleFlash;
	
	UPROPERTY(EditDefaultsOnly)
	TSubclassOf<UCameraShakeBase> CameraShake;

	// Default constructor
	FEffects()
	{
		MuzzleFlashScale = FVector::OneVector;
		MuzzleFlash = nullptr;
	}
};

USTRUCT(BlueprintType)
struct FRecoilData
{
	GENERATED_BODY()
	
	UPROPERTY(EditDefaultsOnly, meta = (ToolTip = "Smaller number = more intensity"))
	FRotator RotationIntensity;
	
	UPROPERTY(EditDefaultsOnly, meta = (ToolTip = "Bigger number = faster control"))
	float ControlTime;
	
	UPROPERTY(EditDefaultsOnly, meta = (ToolTip = "Bigger number = more fedback"))
	float CrosshairRecoil;
	
	UPROPERTY(EditDefaultsOnly, meta = (ToolTip = "Smaller number = more fedback"))
	float ControllerPitch;

	// Default constructor
	FRecoilData()
	{
		RotationIntensity = FRotator(0.0f, 0.0f, -5.0f);
		ControlTime = 0.25f;
		CrosshairRecoil = 10.0f;
		ControllerPitch = -0.5f;
	}
};

UCLASS()
class TPSONLINE_API AWeaponPickupActor : public APickupActor
{
	GENERATED_BODY()

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components", meta = (AllowPrivateAccess = "true"))
	USkeletalMeshComponent* SkeletalMesh;
	
// Functions
public:
	/** Sets default values for this actor's properties */
	AWeaponPickupActor();

	UFUNCTION(Server, Reliable)
	void ServerSpawnProjectile();
	
	virtual void OnRep_PickupState() override;

private:
	void ServerSpawnProjectile_Implementation();

// Variables
public:
	UPROPERTY(EditDefaultsOnly, Category = "Defaults")
	EWeaponType WeaponType;

	UPROPERTY(EditDefaultsOnly, Category = "Defaults")
	EAmmoType AmmoType;

	UPROPERTY(EditDefaultsOnly, Category = "Defaults")
	FEffects Effects;
	
	UPROPERTY(EditDefaultsOnly, Category = "Defaults")
	FRecoilData RecoilData;
};