// All Rights Reserved.

#include "Actors/HealthPickupActor.h"
#include "Net/UnrealNetwork.h"

AHealthPickupActor::AHealthPickupActor()
{
	PrimaryActorTick.bCanEverTick = false;

	MeshComponent = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("Mesh"));
	SetRootComponent(MeshComponent);
	MeshComponent->SetComponentTickEnabled(false);
	MeshComponent->CanCharacterStepUpOn = ECB_No;
	MeshComponent->SetGenerateOverlapEvents(false);
	MeshComponent->SetCollisionProfileName("Pickup");
	
	// Initialize variables
	PickupType = EPickupType::Health;
	IncreaseAmount = 50.0;
}

void AHealthPickupActor::GetLifetimeReplicatedProps(TArray<FLifetimeProperty> &OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	// Replicate to everyone
	DOREPLIFETIME(AHealthPickupActor, IncreaseAmount);
}

void AHealthPickupActor::ServerUpdatePickupState_Implementation(EPickupState NewState)
{
	if (GetLocalRole() == ROLE_Authority)
	{
		switch (NewState)
		{
		case 0:
			// Picked Up
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
}