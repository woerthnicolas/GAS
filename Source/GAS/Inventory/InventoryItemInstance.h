// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "UObject/Object.h"
#include "GameplayAbilitySpec.h"
#include "InventoryItemInstance.generated.h"

class AItemActor;
class UItemStaticData;
/**
 * 
 */
UCLASS(BlueprintType, Blueprintable)
class GAS_API UInventoryItemInstance : public UObject
{
	GENERATED_BODY()
public:
	virtual void Init(TSubclassOf<UItemStaticData> InItemStaticDataClass);

	virtual bool IsSupportedForNetworking() const override { return true; }
	

	UFUNCTION(BlueprintCallable, BlueprintPure)
	const UItemStaticData* GetItemStaticData() const;
	UFUNCTION(BlueprintPure)
	AItemActor* GetItemActor() const;

	UPROPERTY(Replicated)
	TSubclassOf<UItemStaticData> ItemStaticDataClass;

	UPROPERTY(ReplicatedUsing= OnRep_Equipped)
	bool bEquipped = false;

	UFUNCTION()
	void OnRep_Equipped();

	virtual void OnEquipped(AActor* InOwner = nullptr);
	virtual void OnUnequipped(AActor* InOwner = nullptr);
	virtual void OnDropped(AActor* InOwner = nullptr);

protected:
	UPROPERTY(Replicated)
	AItemActor* ItemActor = nullptr;

	void TryGrantAbilities(AActor* InOwner = nullptr);
	void TryRemoveAbilities(AActor* InOwner = nullptr);
	void TryApplyEffects(AActor* InOwner = nullptr);
	void TryRemoveEffects(AActor* InOwner = nullptr);

	UPROPERTY()
	TArray<FGameplayAbilitySpecHandle> GrantedAbilityHandles;

	UPROPERTY(EditDefaultsOnly, Category = "Effects")
	TArray<FActiveGameplayEffectHandle> OngoingEffectHandles;
};
