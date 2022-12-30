// Copyright 2022 Danial Kamali. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "Actors/PickupActor.h"
#include "AmmoPickupActor.generated.h"

UCLASS()
class TPSONLINE_API AAmmoPickupActor : public APickupActor
{
	GENERATED_BODY()

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components", meta = (AllowPrivateAccess = "true"))
	UStaticMeshComponent* MeshComponent;

// Functions
public:
	/** Sets default values for this actor's properties */
	AAmmoPickupActor();

	virtual void OnRep_PickupState() override;

// Variables
public:
	UPROPERTY(EditDefaultsOnly, Category = "Defaults")
	EAmmoType AmmoType;

	UPROPERTY(EditDefaultsOnly, Category = "Defaults")
	int32 Amount;
};