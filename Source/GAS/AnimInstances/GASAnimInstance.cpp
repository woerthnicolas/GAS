// Fill out your copyright notice in the Description page of Project Settings.

#include "AnimInstances/GASAnimInstance.h"
#include "GASCharacter.h"
#include "ActorComponents/InventoryComponent.h"
#include "DataAssets/CharacterAnimDataAsset.h"
#include "Inventory/InventoryItemInstance.h"

const UItemStaticData* UGASAnimInstance::GetEquippedItemData() const
{
	AGASCharacter* Character = Cast<AGASCharacter>(GetOwningActor());
	UInventoryComponent* InventoryComponent = Character ? Character->GetInventoryComponent() : nullptr;
	UInventoryItemInstance* ItemInstance = InventoryComponent ? InventoryComponent->GetEquippedItem() : nullptr;

	return ItemInstance ? ItemInstance->GetItemStaticData() : nullptr;
}

UBlendSpace* UGASAnimInstance::GetLocomotionBlendSpace() const
{
	if (const AGASCharacter* Character = Cast<AGASCharacter>(GetOwningActor()))
	{
		if (const UItemStaticData* EquippedItemData = GetEquippedItemData())
		{
			if (EquippedItemData->CharacterAnimationData.MovementBlendspace)
			{
				return EquippedItemData->CharacterAnimationData.MovementBlendspace;
			}
		}

		const FCharacterData CharacterData = Character->GetCharacterData();
		if (CharacterData.CharacterAnimDataAsset)
		{
			return CharacterData.CharacterAnimDataAsset->CharacterAnimationData.MovementBlendspace;
		}
	}

	return DefaultCharacterAnimDataAsset ? DefaultCharacterAnimDataAsset->CharacterAnimationData.MovementBlendspace : nullptr;
}

UAnimSequenceBase* UGASAnimInstance::GetIdleAnimation() const
{
	if (const AGASCharacter* Character = Cast<AGASCharacter>(GetOwningActor()))
	{
		if (const UItemStaticData* EquippedItemData = GetEquippedItemData())
		{
			if (EquippedItemData->CharacterAnimationData.IdleAnimationAsset)
			{
				return EquippedItemData->CharacterAnimationData.IdleAnimationAsset;
			}
		}

		const FCharacterData CharacterData = Character->GetCharacterData();
		if (CharacterData.CharacterAnimDataAsset)
		{
			return CharacterData.CharacterAnimDataAsset->CharacterAnimationData.IdleAnimationAsset;
		}
	}

	return DefaultCharacterAnimDataAsset ? DefaultCharacterAnimDataAsset->CharacterAnimationData.IdleAnimationAsset : nullptr;
}

UBlendSpace* UGASAnimInstance::GetCrouchLocomotionBlendSpace() const
{
	if (const AGASCharacter* Character = Cast<AGASCharacter>(GetOwningActor()))
	{
		if (const UItemStaticData* EquippedItemData = GetEquippedItemData())
		{
			if (EquippedItemData->CharacterAnimationData.CrouchMovementBlendspace)
			{
				return EquippedItemData->CharacterAnimationData.CrouchMovementBlendspace;
			}
		}

		const FCharacterData CharacterData = Character->GetCharacterData();
		if (CharacterData.CharacterAnimDataAsset)
		{
			return CharacterData.CharacterAnimDataAsset->CharacterAnimationData.CrouchMovementBlendspace;
		}
	}

	return DefaultCharacterAnimDataAsset ? DefaultCharacterAnimDataAsset->CharacterAnimationData.CrouchMovementBlendspace : nullptr;
}

UAnimSequenceBase* UGASAnimInstance::GetCrouchIdleAnimation() const
{
	if (const AGASCharacter* Character = Cast<AGASCharacter>(GetOwningActor()))
	{
		if (const UItemStaticData* EquippedItemData = GetEquippedItemData())
		{
			if (EquippedItemData->CharacterAnimationData.CrouchIdleAnimationAsset)
			{
				return EquippedItemData->CharacterAnimationData.CrouchIdleAnimationAsset;
			}
		}

		const FCharacterData CharacterData = Character->GetCharacterData();
		if (CharacterData.CharacterAnimDataAsset)
		{
			return CharacterData.CharacterAnimDataAsset->CharacterAnimationData.CrouchIdleAnimationAsset;
		}
	}

	return DefaultCharacterAnimDataAsset ? DefaultCharacterAnimDataAsset->CharacterAnimationData.CrouchIdleAnimationAsset : nullptr;
}
