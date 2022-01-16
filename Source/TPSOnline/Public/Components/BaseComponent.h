// All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "BaseComponent.generated.h"

class ABaseCharacter;

UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class TPSONLINE_API UBaseComponent : public UActorComponent
{
	GENERATED_BODY()

// Functions
public:	
	/** Sets default values for this component's properties */
	UBaseComponent();

	/** Initialize this component */
	UFUNCTION(Server, Reliable, WithValidation)
	void ServerInitialize(UBaseComponent* Self);

protected:
	virtual void BeginPlay() override;

	bool ServerInitialize_Validate(UBaseComponent* Self);
	virtual void ServerInitialize_Implementation(UBaseComponent* Self);

// Variables
protected:
	/** Owner of this component */
	UPROPERTY(Replicated)
	ABaseCharacter* ComponentOwner;
};