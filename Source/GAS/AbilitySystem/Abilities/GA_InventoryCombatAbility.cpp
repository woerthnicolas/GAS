﻿// Fill out your copyright notice in the Description page of Project Settings.


#include "GA_InventoryCombatAbility.h"
#include "AbilitySystemComponent.h"
#include "AbilitySystemBlueprintLibrary.h"
#include "ActionGameTypes.h"

FGameplayEffectSpecHandle UGA_InventoryCombatAbility::GetWeaponEffectSpec(const FHitResult& InHitResult)
{
	if (UAbilitySystemComponent* AbilitySystemComponent = GetAbilitySystemComponentFromActorInfo())
	{
		if (const UWeaponStaticData* WeaponStaticData = GetEquippedWeaponStaticData())
		{
			FGameplayEffectContextHandle EffectContext = AbilitySystemComponent->MakeEffectContext();

			FGameplayEffectSpecHandle OutSpec = AbilitySystemComponent->MakeOutgoingSpec(
				WeaponStaticData->DamageEffect, 1, EffectContext);

			UAbilitySystemBlueprintLibrary::AssignTagSetByCallerMagnitude(
				OutSpec, FGameplayTag::RequestGameplayTag(TEXT("Attribute.Health")),
				-WeaponStaticData->BaseDamage);

			return OutSpec;
		}
	}

	return FGameplayEffectSpecHandle();
}
