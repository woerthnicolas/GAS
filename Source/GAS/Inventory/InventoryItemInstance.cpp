﻿// Fill out your copyright notice in the Description page of Project Settings.


#include "InventoryItemInstance.h"

#include "AbilitySystemBlueprintLibrary.h"
#include "AbilitySystemComponent.h"
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

AItemActor* UInventoryItemInstance::GetItemActor() const
{
	return ItemActor;
}

void UInventoryItemInstance::OnRep_Equipped()
{
}

void UInventoryItemInstance::OnEquipped(AActor* InOwner)
{
	if (UWorld* World = InOwner->GetWorld())
	{
		const UItemStaticData* StaticData = GetItemStaticData();

		FTransform Transform;
		ItemActor = World->SpawnActorDeferred<AItemActor>(StaticData->ItemActorClass, Transform, InOwner);;
		ItemActor->Init(this);

		ItemActor->OnEquipped();
		ItemActor->FinishSpawning(Transform);

		ACharacter* Character = Cast<ACharacter>(InOwner);
		if (USkeletalMeshComponent* SkeletalMesh = Character ? Character->GetMesh() : nullptr)
		{
			ItemActor->AttachToComponent(SkeletalMesh, FAttachmentTransformRules::SnapToTargetNotIncludingScale,
			                             StaticData->AttachmentSocket);
		}
	}

	TryGrantAbilities(InOwner);

	TryApplyEffects(InOwner);

	bEquipped = true;
}

void UInventoryItemInstance::OnUnequipped(AActor* InOwner)
{
	if (ItemActor)
	{
		ItemActor->Destroy();
		ItemActor = nullptr;
	}

	TryRemoveAbilities(InOwner);

	TryRemoveEffects(InOwner);

	bEquipped = false;
}

void UInventoryItemInstance::OnDropped(AActor* InOwner)
{
	if (ItemActor)
	{
		ItemActor->OnDropped();
	}

	TryRemoveAbilities(InOwner);

	bEquipped = false;
}

void UInventoryItemInstance::TryGrantAbilities(AActor* InOwner)
{
	if (InOwner && InOwner->HasAuthority())
	{
		if (UAbilitySystemComponent* AbilityComponent =
			UAbilitySystemBlueprintLibrary::GetAbilitySystemComponent(InOwner))
		{
			const UItemStaticData* StaticData = GetItemStaticData();

			for (auto ItemAbility : StaticData->GrantedAbilities)
			{
				GrantedAbilityHandles.Add(AbilityComponent->GiveAbility(FGameplayAbilitySpec(ItemAbility)));
			}
		}
	}
}

void UInventoryItemInstance::TryRemoveAbilities(AActor* InOwner)
{
	if (InOwner && InOwner->HasAuthority())
	{
		if (UAbilitySystemComponent* AbilityComponent =
			UAbilitySystemBlueprintLibrary::GetAbilitySystemComponent(InOwner))
		{
			const UItemStaticData* StaticData = GetItemStaticData();

			for (auto AbilityHandle : GrantedAbilityHandles)
			{
				AbilityComponent->ClearAbility(AbilityHandle);
			}

			GrantedAbilityHandles.Empty();
		}
	}
}

void UInventoryItemInstance::TryApplyEffects(AActor* InOwner)
{
	if (UAbilitySystemComponent* AbilityComponent = UAbilitySystemBlueprintLibrary::GetAbilitySystemComponent(InOwner))
	{
		const UItemStaticData* ItemStaticData = GetItemStaticData();
		
		for (auto GameplayEffect : ItemStaticData->OngoingEffects)
		{
			if (!GameplayEffect.Get()) continue;

			FGameplayEffectContextHandle EffectContext = AbilityComponent->MakeEffectContext();
			FGameplayEffectSpecHandle SpecHandle = AbilityComponent->MakeOutgoingSpec(
				GameplayEffect, 1, EffectContext);
			if (SpecHandle.IsValid())
			{
				FActiveGameplayEffectHandle ActiveGEHandle = AbilityComponent->
				                                                        ApplyGameplayEffectSpecToSelf(
					                                                        *SpecHandle.Data.Get());
				if (!ActiveGEHandle.WasSuccessfullyApplied())
				{
					ABILITY_LOG(Log, TEXT("Item %s failed to apply runtime effect %s"), *GetName(),
					            *GetNameSafe(GameplayEffect));
				}
				else
				{
					OngoingEffectHandles.Add(ActiveGEHandle);
				}
			}
		}
	}
}

void UInventoryItemInstance::TryRemoveEffects(AActor* InOwner)
{
	if (UAbilitySystemComponent* AbilityComponent = UAbilitySystemBlueprintLibrary::GetAbilitySystemComponent(InOwner))
	{
		for (FActiveGameplayEffectHandle ActiveGameplayEffectHandle : OngoingEffectHandles)
		{
			if (ActiveGameplayEffectHandle.IsValid())
			{
				AbilityComponent->RemoveActiveGameplayEffect(ActiveGameplayEffectHandle);
			}
		}
	}
	OngoingEffectHandles.Empty();
}

void UInventoryItemInstance::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	UObject::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME(UInventoryItemInstance, ItemStaticDataClass);
	DOREPLIFETIME(UInventoryItemInstance, bEquipped);
	DOREPLIFETIME(UInventoryItemInstance, ItemActor);
}
