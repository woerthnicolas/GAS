// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "UObject/Object.h"
#include "ActionGameTypes.h"
#include "InventoryList.generated.h"

class UInventoryItemInstance;

USTRUCT(BlueprintType)
struct FInventoryListItem : public FFastArraySerializerItem
{
	GENERATED_BODY()
	
	UPROPERTY()
	UInventoryItemInstance* ItemInstance = nullptr;
};

USTRUCT(BlueprintType)
struct FInventoryList : public FFastArraySerializer
{
	GENERATED_BODY()

	bool NetDeltaSerialize(FNetDeltaSerializeInfo & DeltaParms)
	{
		return FFastArraySerializer::FastArrayDeltaSerialize<FInventoryListItem, FInventoryList>
															( Items, DeltaParms, *this );
	}

	void AddItem(TSubclassOf<UItemStaticData> InItemStaticDataClass);
	void AddItem(UInventoryItemInstance * InItemInstance);
	void RemoveItem(TSubclassOf<UItemStaticData> InItemStaticDataClass);

	TArray<FInventoryListItem>& GetItemsRef() { return Items; }
	
protected:

	TArray<FInventoryListItem> Items;
};

template<>
struct TStructOpsTypeTraits<FInventoryList> : public TStructOpsTypeTraitsBase2<FInventoryList>
{
	enum {WithNetDeltaSerializer = true};
};
