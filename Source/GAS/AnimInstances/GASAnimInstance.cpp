// Fill out your copyright notice in the Description page of Project Settings.

#include "AnimInstances/GASAnimInstance.h"
#include "GASCharacter.h"
#include "DataAssets/CharacterAnimDataAsset.h"

UBlendSpace* UGASAnimInstance::GetLocomotionBlendSpace() const
{
	if(const AGASCharacter* Character = Cast<AGASCharacter>(GetOwningActor()))
	{
		const FCharacterData CharacterData = Character->GetCharacterData();
		if(CharacterData.CharacterAnimDataAsset)
		{
			return CharacterData.CharacterAnimDataAsset->CharacterAnimationData.MovementBlendspace;
		}
	}
	
	return DefaultCharacterAnimDataAsset ? DefaultCharacterAnimDataAsset->CharacterAnimationData.MovementBlendspace : nullptr;
}

UAnimSequenceBase* UGASAnimInstance::GetIdleAnimation() const
{
	if(const AGASCharacter* Character = Cast<AGASCharacter>(GetOwningActor()))
	{
		const FCharacterData CharacterData = Character->GetCharacterData();
		if(CharacterData.CharacterAnimDataAsset)
		{
			return CharacterData.CharacterAnimDataAsset->CharacterAnimationData.IdleAnimationAsset;
		}
	}
	
	return DefaultCharacterAnimDataAsset ? DefaultCharacterAnimDataAsset->CharacterAnimationData.IdleAnimationAsset : nullptr;
}
