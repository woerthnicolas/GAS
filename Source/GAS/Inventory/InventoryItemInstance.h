// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "UObject/Object.h"
#include "InventoryItemInstance.generated.h"

class UItemStaticData;
/**
 * 
 */
UCLASS()
class GAS_API UInventoryItemInstance : public UObject
{
	GENERATED_BODY()
public:
	virtual void Init(TSubclassOf<UItemStaticData> InItemStaticDataClass);
protected:
	virtual bool IsSupportedForNetworking() const override { return true; };

	UPROPERTY(Replicated)
	TSubclassOf<UItemStaticData> ItemStaticDataClass;

	UPROPERTY(ReplicatedUsing= OnRep_Equipped)
	bool bEquipped = false;

	UFUNCTION()
	void OnRep_Equipped();

	virtual void OnEquipped();
	virtual void OnUnequipped();

	virtual void GetLifetimeReplicatedProps(TArray< FLifetimeProperty > & OutLifetimeProps) const override;
};
