// All Rights Reserved.

#pragma once

#include "Engine/DataTable.h"
#include "ExplosiveProjectileInfoStruct.generated.h"

/** Projectile info that projectile class needs */
USTRUCT(BlueprintType)
struct FExplosiveProjectileInfo : public FTableRowBase
{
	GENERATED_BODY()

	FORCEINLINE FExplosiveProjectileInfo();

	explicit FORCEINLINE FExplosiveProjectileInfo(float InBaseDamage, float InMinimumDamage, float InDamageInnerRadius, float InDamageOuterRadius);

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Structs")
	float BaseDamage = 200.0f;

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Structs")
	float MinimumDamage = 50.0f;

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Structs")
	float DamageInnerRadius = 100.0f;

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Structs")
	float DamageOuterRadius = 400.0f;
};

FORCEINLINE FExplosiveProjectileInfo::FExplosiveProjectileInfo()
{
}

FORCEINLINE FExplosiveProjectileInfo::FExplosiveProjectileInfo(const float InBaseDamage, const float InMinimumDamage,
	const float InDamageInnerRadius, const float InDamageOuterRadius) : BaseDamage(InBaseDamage), MinimumDamage(InMinimumDamage),
	DamageInnerRadius(InDamageInnerRadius), DamageOuterRadius(InDamageOuterRadius)
{
}