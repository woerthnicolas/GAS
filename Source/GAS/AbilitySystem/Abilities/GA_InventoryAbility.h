// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Abilities/GameplayAbility.h"
#include "GA_InventoryAbility.generated.h"


class AWeaponItemActor;
class UItemStaticData;
class UInventoryComponent;
class UInventoryItemInstance;
class UWeaponStaticData;
class AItemActor;

UCLASS()
class GAS_API UGA_InventoryAbility : public UGameplayAbility
{
	GENERATED_BODY()

public:
	virtual void OnGiveAbility(const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilitySpec& Spec) override;

	UFUNCTION(BlueprintPure)
	UInventoryComponent* GetInventoryComponent() const;
	
	UFUNCTION(BlueprintPure)
	UInventoryItemInstance* GetEquippedItemInstance() const;
	
	UFUNCTION(BlueprintPure)
	const UItemStaticData* GetEquippedItemStaticData() const;
	
	UFUNCTION(BlueprintPure)
	const UWeaponStaticData* GetEquippedWeaponStaticData() const;
	
	UFUNCTION(BlueprintPure)
	AItemActor* GetEquippedItemActor() const;

	UFUNCTION(BlueprintPure)
	AWeaponItemActor* GetEquippedWeaponItemActor() const;


protected:
	UPROPERTY()
	UInventoryComponent* InventoryComponent = nullptr;
};