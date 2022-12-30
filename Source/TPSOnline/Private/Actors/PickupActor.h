// Copyright 2022 Danial Kamali. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Enumerations/ActorEnums.h"
#include "PickupActor.generated.h"

UCLASS()
class APickupActor : public AActor
{
	GENERATED_BODY()

// Functions
public:
	APickupActor();

	UFUNCTION()
	virtual void OnRep_PickupState();

protected:
	virtual void BeginPlay() override;
	
// Variables
public:
	EPickupType PickupType;

	UPROPERTY(ReplicatedUsing = "OnRep_PickupState")
	EPickupState PickupState;
};
