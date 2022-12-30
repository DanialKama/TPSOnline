// Copyright 2022 Danial Kamali. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Enumerations/ActorEnums.h"
#include "ProjectileActor.generated.h"

class USoundCue;
class UDataTable;

USTRUCT(BlueprintType)
struct FHitEffects
{
	GENERATED_BODY()
	
	UPROPERTY(EditDefaultsOnly)
	UParticleSystem* FleshHitEmitter;

	UPROPERTY(EditDefaultsOnly)
	UParticleSystem* StoneHitEmitter;
	
	UPROPERTY(EditDefaultsOnly)
	UParticleSystem* MetalHitEmitter;
		
	UPROPERTY(EditDefaultsOnly)
	UParticleSystem* WoodHitEmitter;
	
	UPROPERTY(EditDefaultsOnly)
	UParticleSystem* ExplosiveEmitter;
	
	UPROPERTY(EditDefaultsOnly)
	USoundCue* FleshHitSound;
	
	UPROPERTY(EditDefaultsOnly)
	USoundCue* ObjectHitSound;
	
	UPROPERTY(EditDefaultsOnly)
	UMaterialInterface* FleshDecal;
	
	UPROPERTY(EditDefaultsOnly)
	FVector FleshDecalSize;
	
	UPROPERTY(EditDefaultsOnly)
	float FleshDecalLifeSpan;
	
	UPROPERTY(EditDefaultsOnly)
	UMaterialInterface* StoneDecal;

	UPROPERTY(EditDefaultsOnly)
	UMaterialInterface* MetalDecal;

	UPROPERTY(EditDefaultsOnly)
	UMaterialInterface* WoodDecal;
	
	UPROPERTY(EditDefaultsOnly)
	FVector ObjectDecalSize;
	
	UPROPERTY(EditDefaultsOnly)
	float ObjectDecalLifeSpan;

	// Default constructor
	FHitEffects()
	{
		FleshHitEmitter = nullptr;
		WoodHitEmitter = nullptr;
		MetalHitEmitter = nullptr;
		StoneHitEmitter = nullptr;
		ExplosiveEmitter = nullptr;
		FleshHitSound = nullptr;
		ObjectHitSound = nullptr;
		FleshDecal = nullptr;
		FleshDecalSize = FVector(1.0f, 5.0f, 5.0f);
		FleshDecalLifeSpan = 20.0f;
		WoodDecal = nullptr;
		MetalDecal = nullptr;
		StoneDecal = nullptr;
		ObjectDecalSize = FVector(5.0f, 10.0f, 10.0f);
		ObjectDecalLifeSpan = 10.0f;
	}
};

UCLASS()
class AProjectileActor : public AActor
{
	GENERATED_BODY()

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components", meta = (AllowPrivateAccess = "true"))
	UStaticMeshComponent* StaticMesh;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components", meta = (AllowPrivateAccess = "true"))
	UParticleSystemComponent* TrailParticle;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components", meta = (AllowPrivateAccess = "true"))
	class UProjectileMovementComponent* ProjectileMovement;

// Functions
public:
	AProjectileActor();

protected:
	virtual void BeginPlay() override;

private:
	UFUNCTION()
	void OnHit(UPrimitiveComponent* HitComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, FVector NormalImpulse, const FHitResult& Hit);

	UFUNCTION(NetMulticast, Unreliable)
	void MulticastHitEffects(uint8 InSwitchExpression, FHitResult HitResult);
	void MulticastHitEffects_Implementation(uint8 InSwitchExpression, FHitResult HitResult);

	/** Calculating the point damage that needs to be applied based on the surface type. */
	float CalculatePointDamage(const struct FProjectileInfo* ProjectileInfo) const;

	/** Returns effects that need to spawn based on the surface type. */
	void FindHitEffects(uint8 InSwitchExpression, UParticleSystem*& Emitter, USoundCue*& Sound, UMaterialInterface*& Decal, FVector& DecalSize, float& DecalLifeSpan) const;

// Variables
public:
	UPROPERTY(EditDefaultsOnly, Category = "Defaults", meta = (AllowPrivateAccess = true))
	int32 NumberOfPellets;

	UPROPERTY(EditDefaultsOnly, Category = "Defaults", meta = (AllowPrivateAccess = true))
	int32 PelletSpread;
	
private:
	UPROPERTY(EditDefaultsOnly, Category = "Defaults", meta = (AllowPrivateAccess = true))
	EAmmoType AmmoType;

	UPROPERTY(EditDefaultsOnly, Category = "Defaults", meta = (AllowPrivateAccess = "true"))
	TSubclassOf<UDamageType> DamageType;

	UPROPERTY(EditDefaultsOnly, Category = "Defaults", meta = (AllowPrivateAccess = true))
	uint8 bIsExplosive : 1;
	
	UPROPERTY(EditDefaultsOnly, Category = "Defaults", meta = (AllowPrivateAccess = true))
	float LifeSpan;

	UPROPERTY(EditDefaultsOnly, Category = "Defaults", meta = (ToolTip = "Non-explosive projectiles", AllowPrivateAccess = true))
	UDataTable* ProjectileDataTable;

	UPROPERTY(EditDefaultsOnly, Category = "Defaults", meta = (AllowPrivateAccess = true))
	UDataTable* ExplosiveProjectileDataTable;

	UPROPERTY(EditDefaultsOnly, Category = "Defaults", meta = (AllowPrivateAccess = "true"))
	FHitEffects HitEffects;

	/** To use in all functions that switch on the surface type */
	uint8 SwitchExpression;
};
