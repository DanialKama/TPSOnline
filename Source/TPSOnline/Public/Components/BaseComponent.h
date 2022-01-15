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

	virtual void Initialize();

// Variables
protected:
	/** Owner of this component */
	UPROPERTY()
	ABaseCharacter* ComponentOwner;
};