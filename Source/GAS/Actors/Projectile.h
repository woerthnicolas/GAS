// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Projectile.generated.h"

class UProjectileStaticData;
class UProjectileMovementComponent;

UCLASS()
class GAS_API AProjectile : public AActor
{
	GENERATED_BODY()

public:
	// Sets default values for this actor's properties
	AProjectile();

	const UProjectileStaticData* GetProjectileStaticData() const;

	UPROPERTY(BlueprintReadOnly, Replicated)
	TSubclassOf<UProjectileStaticData> ProjectileDataClass;
	
protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

	virtual void EndPlay(const EEndPlayReason::Type EndPlayReason) override;

	UPROPERTY()
	UProjectileMovementComponent* ProjectileMovementComponent = nullptr;

	void DebugDrawPath() const;

	UPROPERTY()
	UStaticMeshComponent* StaticMeshComponent = nullptr;

	UFUNCTION()
	void OnProjectileStop(const FHitResult& ImpactResult);

public:
	// Called every frame
	virtual void Tick(float DeltaTime) override;
};
