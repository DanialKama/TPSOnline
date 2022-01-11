// All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "PickupActor.generated.h"

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
	
public:	
	/** Sets default values for this actor's properties */
	APickupActor();

	virtual void UpdatePickupState(EPickupState NewState);
};