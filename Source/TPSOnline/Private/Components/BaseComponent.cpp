// All Rights Reserved.

#include "Components/BaseComponent.h"

#include "Characters/BaseCharacter.h"

UBaseComponent::UBaseComponent()
{
	PrimaryComponentTick.bCanEverTick = false;
}

void UBaseComponent::Initialize()
{
	ComponentOwner = Cast<ABaseCharacter>(GetOwner());
}