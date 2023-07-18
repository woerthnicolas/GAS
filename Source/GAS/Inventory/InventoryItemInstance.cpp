﻿// Fill out your copyright notice in the Description page of Project Settings.


#include "InventoryItemInstance.h"

#include "GASBlueprintFunctionLibrary.h"
#include "Actors/ItemActor.h"
#include "GameFramework/Character.h"
#include "Net/UnrealNetwork.h"

void UInventoryItemInstance::Init(TSubclassOf<UItemStaticData> InItemStaticDataClass)
{
	ItemStaticDataClass = InItemStaticDataClass;
}

const UItemStaticData* UInventoryItemInstance::GetItemStaticData() const
{
	return UGASBlueprintFunctionLibrary::GetItemStaticData(ItemStaticDataClass);
}

void UInventoryItemInstance::OnRep_Equipped()
{
	
}

void UInventoryItemInstance::OnEquipped(AActor* InOwner)
{
	if(UWorld* World = InOwner->GetWorld())
	{
		const UItemStaticData* StaticData = GetItemStaticData();
		
		FTransform Transform;
		ItemActor = World->SpawnActorDeferred<AItemActor>(StaticData->ItemActorClass, Transform, InOwner);;
		ItemActor->Init(this);

		ItemActor->FinishSpawning(Transform);

		ACharacter* Character = Cast<ACharacter>(InOwner);
		if(USkeletalMeshComponent* SkeletalMesh = Character ? Character->GetMesh() : nullptr)
		{
			ItemActor->AttachToComponent(SkeletalMesh, FAttachmentTransformRules::SnapToTargetNotIncludingScale, StaticData->AttachmentSocket);
		}
	}	
}

void UInventoryItemInstance::OnUnequipped()
{
	if(ItemActor)
	{
		ItemActor->Destroy();
		ItemActor = nullptr;
	}
}

void UInventoryItemInstance::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	UObject::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME(UInventoryItemInstance, ItemStaticDataClass);
	DOREPLIFETIME(UInventoryItemInstance, bEquipped);
	DOREPLIFETIME(UInventoryItemInstance, ItemActor);
}


