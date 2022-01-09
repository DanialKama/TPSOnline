// All Rights Reserved.

#include "Components/BaseComponent.h"
#include "Net/UnrealNetwork.h"

// Sets default values for this component's properties
UBaseComponent::UBaseComponent()
{
	PrimaryComponentTick.bCanEverTick = false;
	SetIsReplicated(true);
}

void UBaseComponent::BeginPlay()
{
	Super::BeginPlay();
}