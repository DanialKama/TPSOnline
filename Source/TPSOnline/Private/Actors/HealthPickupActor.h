// Copyright 2022 Danial Kamali. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "PickupActor.h"
#include "HealthPickupActor.generated.h"

UCLASS()
class AHealthPickupActor : public APickupActor
{
	GENERATED_BODY()
	
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components", meta = (AllowPrivateAccess = "true"))
	UStaticMeshComponent* MeshComponent;

// Functions
public:
	AHealthPickupActor();

	virtual void OnRep_PickupState() override;
	
// Variables
public:
	UPROPERTY(Replicated, EditDefaultsOnly, Category = "Defaults", meta = (ClampMin = "0.0", UIMin = "0.0"))
	float IncreaseAmount;
};
