// Fill out your copyright notice in the Description page of Project Settings.


#include "GASBlueprintFunctionLibrary.h"

const UItemStaticData* UGASBlueprintFunctionLibrary::GetItemStaticData(TSubclassOf<UItemStaticData> ItemDataClass)
{
	if(IsValid(ItemDataClass))
	{
		return GetDefault<UItemStaticData>(ItemDataClass);
	}

	return nullptr;
}
