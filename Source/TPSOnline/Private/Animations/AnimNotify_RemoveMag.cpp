// Copyright 2022 Danial Kamali. All Rights Reserved.

#include "Animations/AnimNotify_RemoveMag.h"
#include "Characters/BaseCharacter.h"

void UAnimNotify_RemoveMag::Notify(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation)
{
	if (MeshComp && MeshComp->GetOwner())
	{
		ABaseCharacter* Character = Cast<ABaseCharacter>(MeshComp->GetOwner());
		if (Character)
		{
			// Character->SetReloadNotify(EReloadState::RemoveMag);
		}
	}
}