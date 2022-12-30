// Copyright 2022 Danial Kamali. All Rights Reserved.

#include "HealthPickupActor.h"

#include "Net/UnrealNetwork.h"

AHealthPickupActor::AHealthPickupActor()
{
	MeshComponent = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("Mesh"));
	RootComponent = MeshComponent;
	MeshComponent->SetComponentTickEnabled(false);
	MeshComponent->CanCharacterStepUpOn = ECB_No;
	MeshComponent->SetGenerateOverlapEvents(false);
	MeshComponent->SetCollisionProfileName("Pickup");
	
	// Initialize variables
	PickupType = EPickupType::Health;
	IncreaseAmount = 50.0;
}

void AHealthPickupActor::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	// Replicate to everyone
	DOREPLIFETIME(AHealthPickupActor, IncreaseAmount);
}

void AHealthPickupActor::OnRep_PickupState()
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
		Destroy();
		break;
	}
}