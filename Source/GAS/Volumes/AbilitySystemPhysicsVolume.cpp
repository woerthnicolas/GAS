// Fill out your copyright notice in the Description page of Project Settings.


#include "AbilitySystemPhysicsVolume.h"

#include "AbilitySystemBlueprintLibrary.h"
#include "AbilitySystemComponent.h"

AAbilitySystemPhysicsVolume::AAbilitySystemPhysicsVolume()
{
	PrimaryActorTick.bCanEverTick = true;
}

void AAbilitySystemPhysicsVolume::ActorEnteredVolume(AActor* Other)
{
	Super::ActorEnteredVolume(Other);

	if (!HasAuthority()) return;

	if (UAbilitySystemComponent* ASC = UAbilitySystemBlueprintLibrary::GetAbilitySystemComponent(Other))
	{
		for (auto GA : PermanentAbilitiesToGive)
		{
			ASC->GiveAbility(FGameplayAbilitySpec(GA));
		}

		EnteredActorsInfoMap.Add(Other);

		for (auto GA : OngoingAbilitiesToGive)
		{
			FGameplayAbilitySpecHandle AbilityHandle = ASC->GiveAbility(FGameplayAbilitySpec(GA));

			EnteredActorsInfoMap[Other].AppliedAbilities.Add(AbilityHandle);
		}

		for (auto GE : OngoingEffectsToApply)
		{
			FGameplayEffectContextHandle EffectContextHandle = ASC->MakeEffectContext();

			EffectContextHandle.AddInstigator(Other, Other);

			FGameplayEffectSpecHandle SpecHandle = ASC->MakeOutgoingSpec(GE, 1, EffectContextHandle);
			if (SpecHandle.IsValid())
			{
				FActiveGameplayEffectHandle ActiveGEHandle = ASC->ApplyGameplayEffectSpecToSelf(*SpecHandle.Data.Get());

				if (ActiveGEHandle.WasSuccessfullyApplied())
				{
					EnteredActorsInfoMap[Other].AppliedEffects.Add(ActiveGEHandle);
				}
			}
		}
		for (auto EventTag : GameplayEventsToSendOnEnter)
		{
			FGameplayEventData EventPayload;
			EventPayload.EventTag = EventTag;

			UAbilitySystemBlueprintLibrary::SendGameplayEventToActor(Other, EventTag, EventPayload);
		}
	}
}

void AAbilitySystemPhysicsVolume::ActorLeavingVolume(AActor* Other)
{
	Super::ActorLeavingVolume(Other);

	if (!HasAuthority()) return;

	if (UAbilitySystemComponent* ASC = UAbilitySystemBlueprintLibrary::GetAbilitySystemComponent(Other))
	{
		if (EnteredActorsInfoMap.Find(Other))
		{
			for (auto GEHandle : EnteredActorsInfoMap[Other].AppliedEffects)
			{
				ASC->RemoveActiveGameplayEffect(GEHandle);
			}

			for (auto GAHandle : EnteredActorsInfoMap[Other].AppliedAbilities)
			{
				ASC->ClearAbility(GAHandle);
			}

			for (auto GE : OnExitEffectsToApply)
			{
				FGameplayEffectContextHandle EffectContextHandle = ASC->MakeEffectContext();

				EffectContextHandle.AddInstigator(Other, Other);

				FGameplayEffectSpecHandle SpecHandle = ASC->MakeOutgoingSpec(GE, 1, EffectContextHandle);
				if (SpecHandle.IsValid())
				{
					FActiveGameplayEffectHandle ActiveGEHandle = ASC->ApplyGameplayEffectSpecToSelf(
						*SpecHandle.Data.Get());

					if (ActiveGEHandle.WasSuccessfullyApplied())
					{
						EnteredActorsInfoMap[Other].AppliedEffects.Add(ActiveGEHandle);
					}
				}
			}
			
			EnteredActorsInfoMap.Remove(Other);

			for (auto EventTag : GameplayEventsToSendOnExit)
			{
				FGameplayEventData EventPayload;
				EventPayload.EventTag = EventTag;

				UAbilitySystemBlueprintLibrary::SendGameplayEventToActor(Other, EventTag, EventPayload);
			}
		}
	}
}

void AAbilitySystemPhysicsVolume::Tick(float DeltaSeconds)
{
	Super::Tick(DeltaSeconds);

	if (bDrawDebug)
	{
		DrawDebugBox(GetWorld(), GetActorLocation(), GetBounds().BoxExtent, FColor::Red, false, 0, 0, 5);
	}
}
