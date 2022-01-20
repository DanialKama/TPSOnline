// All Rights Reserved.

#include "Components/BaseComponent.h"
#include "Characters/BaseCharacter.h"
#include "Net/UnrealNetwork.h"

UBaseComponent::UBaseComponent()
{
	PrimaryComponentTick.bCanEverTick = false;
}

void UBaseComponent::GetLifetimeReplicatedProps(TArray<FLifetimeProperty> &OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	// Replicate to everyone
	DOREPLIFETIME(UBaseComponent, ComponentOwner);
}

void UBaseComponent::BeginPlay()
{
	Super::BeginPlay();

	if (GetOwnerRole() == ROLE_Authority)
	{
		SetIsReplicated(true);
	}
}

void UBaseComponent::ServerInitialize_Implementation(UBaseComponent* Self)
{
	if (GetOwnerRole() == ROLE_Authority)
	{
		ComponentOwner = Cast<ABaseCharacter>(Self->GetOwner());
	}
}