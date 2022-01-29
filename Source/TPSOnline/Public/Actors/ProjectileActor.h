// All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Enumerations/ActorEnums.h"
#include "ProjectileActor.generated.h"

UCLASS()
class TPSONLINE_API AProjectileActor : public AActor
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
	/** Sets default values for this actor's properties */
	AProjectileActor();

protected:
	/** Called when the game starts or when spawned */
	virtual void BeginPlay() override;

private:
	UFUNCTION()
	void OnHit(UPrimitiveComponent* HitComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, FVector NormalImpulse, const FHitResult& Hit);

	UFUNCTION(NetMulticast, Unreliable)
	void MulticastHitEffects(FHitResult HitResult);
	void MulticastHitEffects_Implementation(FHitResult HitResult);

// Variables
public:
	UPROPERTY(EditDefaultsOnly, Category = "Defaults", meta = (AllowPrivateAccess = true))
	int32 NumberOfPellets;

	UPROPERTY(EditDefaultsOnly, Category = "Defaults", meta = (AllowPrivateAccess = true))
	int32 PelletSpread;
	
private:
	UPROPERTY(EditDefaultsOnly, Category = "Defaults", meta = (AllowPrivateAccess = true))
	EAmmoType AmmoType;

	UPROPERTY(EditDefaultsOnly, Category = "Defaults", meta = (AllowPrivateAccess = true))
	uint8 bIsExplosive : 1;
	
	UPROPERTY(EditDefaultsOnly, Category = "Defaults", meta = (AllowPrivateAccess = true))
	float LifeSpan;
};