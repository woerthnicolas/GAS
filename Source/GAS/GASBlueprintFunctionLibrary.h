// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Kismet/BlueprintFunctionLibrary.h"
#include "ActionGameTypes.h"
#include "GASBlueprintFunctionLibrary.generated.h"

class AProjectile;
/**
 * 
 */
UCLASS()
class GAS_API UGASBlueprintFunctionLibrary : public UBlueprintFunctionLibrary
{
	GENERATED_BODY()

public:
	UFUNCTION(BlueprintCallable, BlueprintPure)
	static const UItemStaticData* GetItemStaticData(TSubclassOf<UItemStaticData> ItemDataClass);

	UFUNCTION(BlueprintCallable, meta = (WorldContext = "WorldContextObject"))
	static void ApplyRadialDamage(UObject* WorldContextObject, AActor* DamageCauser, FVector Location,
	                              float Radius, float DamageAmount,
	                              TArray<TSubclassOf<class UGameplayEffect>> DamageEffects,
	                              const TArray<TEnumAsByte<EObjectTypeQuery>>& ObjectTypes, ETraceTypeQuery TraceType);

	UFUNCTION(BlueprintCallable, meta = (WorldContext = "WorldContextObject"))
	static AProjectile* LaunchProjectile(UObject* WorldContextObject,
	                                     TSubclassOf<UProjectileStaticData> ProjectileStaticData, FTransform Transform,
	                                     AActor* Owner, APawn* Instigator);
};
