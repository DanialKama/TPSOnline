// All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "Actors/PickupActor.h"
#include "WeaponPickupActor.generated.h"

UENUM()
enum class EWeaponType : uint8
{
	Pistol,
	SMG,
	Rifle,
	LMG,
	Shotgun,
	Sniper,
	Launcher
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
	
private:
	virtual void ServerUpdatePickupState_Implementation(EPickupState NewState) override;

// Variables
public:
	UPROPERTY(EditDefaultsOnly, Category = "Defaults")
	EWeaponType WeaponType;
};