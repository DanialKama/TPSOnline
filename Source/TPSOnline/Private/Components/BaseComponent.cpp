// All Rights Reserved.

#include "Components/BaseComponent.h"
#include "Characters/BaseCharacter.h"

UBaseComponent::UBaseComponent()
{
	PrimaryComponentTick.bCanEverTick = false;
}

void UBaseComponent::BeginPlay()
{
	Super::BeginPlay();

	if (GetOwnerRole() == ROLE_Authority)
	{
		SetIsReplicated(true);
	}
}

void UBaseComponent::Initialize()
{
	ComponentOwner = Cast<ABaseCharacter>(GetOwner());
}