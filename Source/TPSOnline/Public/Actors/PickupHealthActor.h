// All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "Actors/PickupActor.h"
#include "PickupHealthActor.generated.h"

UCLASS()
class TPSONLINE_API APickupHealthActor : public APickupActor
{
	GENERATED_BODY()
	
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components", meta = (AllowPrivateAccess = "true"))
	UStaticMeshComponent* MeshComponent;
	
public:
	APickupHealthActor();

	virtual void UpdatePickupState(EPickupState NewState) override;
};