// All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "Actors/PickupActor.h"
#include "WeaponPickupActor.generated.h"

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

	virtual void OnRep_PickupState() override;
	
// Variables
public:
	UPROPERTY(EditDefaultsOnly, Category = "Defaults")
	EWeaponType WeaponType;

	UPROPERTY(EditDefaultsOnly, Category = "Defaults")
	EAmmoType AmmoType;
};