﻿// Fill out your copyright notice in the Description page of Project Settings.


#include "GA_InventoryCombatAbility.h"
#include "AbilitySystemComponent.h"
#include "AbilitySystemBlueprintLibrary.h"
#include "ActionGameTypes.h"
#include "GASCharacter.h"
#include "Camera/CameraComponent.h"
#include "Inventory/ItemActors/WeaponItemActor.h"

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
	                                      ActorsToIgnore, EDrawDebugTrace::None, FocusHit, true);

	FVector MuzzleLocation = WeaponItemActor->GetMuzzleLocation();

	const FVector WeaponTraceEnd = MuzzleLocation + (FocusHit.Location - MuzzleLocation).GetSafeNormal() * TraceDistance;

	UKismetSystemLibrary::LineTraceSingle(this, CameraTransform.GetLocation(), WeaponTraceEnd, TraceType, false,
										  ActorsToIgnore, EDrawDebugTrace::None, OutHitResult, true);
	
	return OutHitResult.bBlockingHit;
}
