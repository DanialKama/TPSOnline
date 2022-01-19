// All Rights Reserved.

#include "Core/DeathmatchGameMode.h"
#include "Characters/PlayerCharacter.h"

void ADeathmatchGameMode::ServerStartRespawn_Implementation(AController* Controller)
{
	if (GetLocalRole() == ROLE_Authority && Controller)
	{
		ControllersToRespawn.Add(Controller);

		FTimerDelegate TimerDelegate;
		TimerDelegate.BindUObject(this, &ADeathmatchGameMode::ServerRespawn);
		GetWorld()->GetTimerManager().SetTimerForNextTick(TimerDelegate);
	}
}

bool ADeathmatchGameMode::ServerRespawn_Validate()
{
	if (ControllersToRespawn.Num() > 0)
	{
		return true;
	}
	return false;
}

void ADeathmatchGameMode::ServerRespawn_Implementation()	// TODO - Test loops
{
	if (GetLocalRole() == ROLE_Authority)
	{
		// Check if the player is still in the session
		if (ExitedControllers.Num() > 0)
		{
			for (uint8 i = 0; i < ControllersToRespawn.Num(); ++i)
			{
				for (uint8 j = 0; j < ExitedControllers.Num(); ++j)
				{
					// Exited players will be removed from the controllers to respawn and will not get respawned
					if (ControllersToRespawn[i] == ExitedControllers[j])
					{
						ControllersToRespawn.RemoveAt(i);
					}

					ExitedControllers.RemoveAt(j);
				}
			}
		}

		if (ControllersToRespawn.Num() > 0)
		{
			for (uint8 k = 0; k < ControllersToRespawn.Num(); ++k)
			{
				const AActor* RespawnActor = ChoosePlayerStart(ControllersToRespawn[k]);
				const FVector Location = RespawnActor->GetActorLocation();
				const FRotator Rotation = RespawnActor->GetActorRotation();
				FActorSpawnParameters SpawnParameters;
				SpawnParameters.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AdjustIfPossibleButAlwaysSpawn;
				APawn* Player = GetWorld()->SpawnActor<APawn>(DefaultPawnClass, Location, Rotation, SpawnParameters);
				if (Player)
				{
					ControllersToRespawn[k]->Possess(Player);
				}

				ControllersToRespawn.RemoveAt(k);
			}
		}
	}
}

void ADeathmatchGameMode::Logout(AController* Exiting)
{
	Super::Logout(Exiting);

	ExitedControllers.Add(Exiting);
}