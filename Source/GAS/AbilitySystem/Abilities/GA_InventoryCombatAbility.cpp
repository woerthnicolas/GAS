﻿// Fill out your copyright notice in the Description page of Project Settings.


#include "GA_InventoryCombatAbility.h"
#include "AbilitySystemComponent.h"
#include "AbilitySystemBlueprintLibrary.h"
#include "ActionGameTypes.h"
#include "GASCharacter.h"
#include "ActorComponents/InventoryComponent.h"
#include "Camera/CameraComponent.h"
#include "Inventory/ItemActors/WeaponItemActor.h"

bool UGA_InventoryCombatAbility::CommitAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo, FGameplayTagContainer* OptionalRelevantTags)
{
	return Super::CommitAbility(Handle, ActorInfo, ActivationInfo, OptionalRelevantTags) && HasEnoughAmmo();
}

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

const bool UGA_InventoryCombatAbility::GetWeaponToFocusTraceResult(float TraceDistance, ETraceTypeQuery TraceType,
                                                                   FHitResult& OutHitResult)
{
	AWeaponItemActor* WeaponItemActor = GetEquippedWeaponItemActor();

	AGASCharacter* Character = GetCharacterFromActorInfo();

	const FTransform& CameraTransform = Character->GetFollowCamera()->GetComponentTransform();

	const FVector FocusTraceEnd = CameraTransform.GetLocation() + CameraTransform.GetRotation().Vector() * TraceDistance;

	TArray<AActor*> ActorsToIgnore = {GetAvatarActorFromActorInfo()};

	FHitResult FocusHit;

	UKismetSystemLibrary::LineTraceSingle(this, CameraTransform.GetLocation(), FocusTraceEnd, TraceType, false,
	                                      ActorsToIgnore, EDrawDebugTrace::ForDuration, FocusHit, true);

	FVector MuzzleLocation = WeaponItemActor->GetMuzzleLocation();

	const FVector WeaponTraceEnd = MuzzleLocation + (FocusHit.Location - MuzzleLocation).GetSafeNormal() * TraceDistance;

	UKismetSystemLibrary::LineTraceSingle(this, MuzzleLocation, WeaponTraceEnd, TraceType, false,
										  ActorsToIgnore, EDrawDebugTrace::ForDuration, OutHitResult, true);
	
	return OutHitResult.bBlockingHit;
}

bool UGA_InventoryCombatAbility::HasEnoughAmmo() const
{
	if(const UWeaponStaticData* WeaponStaticData = GetEquippedWeaponStaticData())
	{
		if(UInventoryComponent* Inventory = GetInventoryComponent())
		{
			return !WeaponStaticData->AmmoTag.IsValid() || Inventory->GetInventoryTagCount(WeaponStaticData->AmmoTag) > 0;
		}
	}

	return false;
}

void UGA_InventoryCombatAbility::DecAmmo()
{
	if(const UWeaponStaticData* WeaponStaticData = GetEquippedWeaponStaticData())
	{
		if(!WeaponStaticData->AmmoTag.IsValid()) return;
		
		if(UInventoryComponent* Inventory = GetInventoryComponent())
		{
			Inventory->RemoveItemWithInventoryTag(WeaponStaticData->AmmoTag, 1);
		}
	}
}
