// Copyright 2022 Danial Kamali. All Rights Reserved.

#include "PickupActor.h"

#include "Net/UnrealNetwork.h"

APickupActor::APickupActor()
{
	PrimaryActorTick.bCanEverTick = false;
	PrimaryActorTick.bStartWithTickEnabled = false;

	// Initialize variables
	PickupState = EPickupState::Dropped;
}

void APickupActor::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	// Replicate to everyone
	DOREPLIFETIME(APickupActor, PickupState);
}

void APickupActor::BeginPlay()
{
	Super::BeginPlay();

	if (GetLocalRole() == ROLE_Authority)
	{
		SetReplicates(true);
	}
}

void APickupActor::OnRep_PickupState()
{
	switch (PickupState)
	{
	case 0:
		// Picked up
		break;
	case 1:
		// Dropped
		break;
	case 2:
		// Used
		break;
	}
}