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

	UFUNCTION(Server, Reliable)
	void ServerUpdatePickupState(EPickupState NewState);

protected:
	virtual void BeginPlay() override;

	virtual void ServerUpdatePickupState_Implementation(EPickupState NewState);
	
// Variables
public:
	EPickupType PickupType;
};