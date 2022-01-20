// All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "Actors/PickupActor.h"
#include "WeaponPickupActor.generated.h"

UCLASS()
class TPSONLINE_API AWeaponPickupActor : public APickupActor
{
	GENERATED_BODY()

// Functions
public:
	/** Sets default values for this actor's properties */
	AWeaponPickupActor();
	
private:
	virtual void ServerUpdatePickupState_Implementation(APickupActor* Self, EPickupState NewState) override;
};