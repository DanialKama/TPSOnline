// All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "Actors/PickupActor.h"
#include "HealthPickupActor.generated.h"

UCLASS()
class TPSONLINE_API AHealthPickupActor : public APickupActor
{
	GENERATED_BODY()
	
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components", meta = (AllowPrivateAccess = "true"))
	UStaticMeshComponent* MeshComponent;

// Functions
public:
	/** Sets default values for this actor's properties */
	AHealthPickupActor();

	virtual void OnRep_PickupState() override;
	
// Variables
public:
	UPROPERTY(Replicated, EditDefaultsOnly, Category = "Defaults", meta = (ClampMin = "0.0", UIMin = "0.0"))
	float IncreaseAmount;
};