// All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "PickupActor.generated.h"

UENUM()
enum class EPickupType : uint8
{
	Weapon,
	Ammo,
	Health
};

UENUM()
enum class EPickupState : uint8
{
	PickedUp,
	Dropped,
	Used
};

UCLASS()
class TPSONLINE_API APickupActor : public AActor
{
	GENERATED_BODY()

// Functions
public:	
	/** Sets default values for this actor's properties */
	APickupActor();

	UFUNCTION()
	virtual void OnRep_UpdatePickupState();

protected:
	virtual void BeginPlay() override;
	
// Variables
public:
	EPickupType PickupType;

	UPROPERTY(ReplicatedUsing = OnRep_UpdatePickupState)
	EPickupState PickupState;
};