// Copyright 2022 Danial Kamali. All Rights Reserved.

#pragma once

#include "Engine/DataTable.h"
#include "ProjectileInfoStruct.generated.h"

/** Projectile info that projectile class needs */
USTRUCT(BlueprintType)
struct FProjectileInfo : public FTableRowBase
{
	GENERATED_BODY()

	FORCEINLINE FProjectileInfo();

	explicit FORCEINLINE FProjectileInfo(float InDefaultDamage, float InDamageToHead, float InDamageToBody,
		float InDamageToHand, float InDamageToLeg);

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Structs")
	float DefaultDamage = 75.0f;
	
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Structs")
	float DamageToHead = 100.0f;

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Structs")
	float DamageToBody = 50.0f;

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Structs")
	float DamageToArm = 25.0f;

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Structs")
	float DamageToLeg = 25.0f;
};

FORCEINLINE FProjectileInfo::FProjectileInfo()
{
}

FORCEINLINE FProjectileInfo::FProjectileInfo(const float InDefaultDamage, const float InDamageToHead, const float InDamageToBody,
	const float InDamageToHand, const float InDamageToLeg) : DefaultDamage(InDefaultDamage) ,DamageToHead(InDamageToHead), DamageToBody(InDamageToBody),
	DamageToArm(InDamageToHand), DamageToLeg(InDamageToLeg)
{
}