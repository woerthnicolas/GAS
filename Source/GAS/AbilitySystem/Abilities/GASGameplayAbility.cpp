// Fill out your copyright notice in the Description page of Project Settings.


#include "GASGameplayAbility.h"

#include "AbilitySystemComponent.h"
#include "GASCharacter.h"

void UGASGameplayAbility::ActivateAbility(const FGameplayAbilitySpecHandle Handle,
                                          const FGameplayAbilityActorInfo* ActorInfo,
                                          const FGameplayAbilityActivationInfo ActivationInfo,
                                          const FGameplayEventData* TriggerEventData)
{
	Super::ActivateAbility(Handle, ActorInfo, ActivationInfo, TriggerEventData);

	FGameplayEffectContextHandle EffectContext = ActorInfo->AbilitySystemComponent->MakeEffectContext();

	for (auto GameplayEffect : OngoingEffectsToJustApplyOnStart)
	{
		if (!GameplayEffect.Get()) continue;

		if (UAbilitySystemComponent* AbilitySystemComponent = ActorInfo->AbilitySystemComponent.Get())
		{
			FGameplayEffectSpecHandle SpecHandle = AbilitySystemComponent->MakeOutgoingSpec(
				GameplayEffect, 1, EffectContext);
			if (SpecHandle.IsValid())
			{
				FActiveGameplayEffectHandle ActiveGEHandle = ActorInfo->AbilitySystemComponent->
				                                                        ApplyGameplayEffectSpecToSelf(
					                                                        *SpecHandle.Data.Get());
				if (!ActiveGEHandle.WasSuccessfullyApplied())
				{
					ABILITY_LOG(Log, TEXT("Ability %s failed to apply startup effect %s"), *GetName(),
					            *GetNameSafe(GameplayEffect));
				}
			}
		}
	}

	if (IsInstantiated())
	{
		for (auto GameplayEffect : OngoingEffectsToRemoveOnEnd)
		{
			if (!GameplayEffect.Get()) continue;

			if (UAbilitySystemComponent* AbilitySystemComponent = ActorInfo->AbilitySystemComponent.Get())
			{
				FGameplayEffectSpecHandle SpecHandle = AbilitySystemComponent->MakeOutgoingSpec(
					GameplayEffect, 1, EffectContext);
				if (SpecHandle.IsValid())
				{
					FActiveGameplayEffectHandle ActiveGEHandle = ActorInfo->AbilitySystemComponent->
					                                                        ApplyGameplayEffectSpecToSelf(
						                                                        *SpecHandle.Data.Get());
					if (!ActiveGEHandle.WasSuccessfullyApplied())
					{
						ABILITY_LOG(Log, TEXT("Ability %s failed to apply runtime effect %s"), *GetName(),
						            *GetNameSafe(GameplayEffect));
					}
					else
					{
						RemoveOnEndEffectHandles.Add(ActiveGEHandle);
					}
				}
			}
		}
	}
}

void UGASGameplayAbility::EndAbility(const FGameplayAbilitySpecHandle Handle,
                                     const FGameplayAbilityActorInfo* ActorInfo,
                                     const FGameplayAbilityActivationInfo ActivationInfo,
                                     bool bReplicateEndAbility, bool bWasCancelled)
{
	if (IsInstantiated())
	{
		for (FActiveGameplayEffectHandle ActiveGameplayEffectHandle : RemoveOnEndEffectHandles)
		{
			if (ActiveGameplayEffectHandle.IsValid())
			{
				ActorInfo->AbilitySystemComponent->RemoveActiveGameplayEffect(ActiveGameplayEffectHandle);
			}
		}

		RemoveOnEndEffectHandles.Empty();
	}
	Super::EndAbility(Handle, ActorInfo, ActivationInfo, bReplicateEndAbility, bWasCancelled);
}

AGASCharacter* UGASGameplayAbility::GetCharacterFromActorInfo() const
{
	return Cast<AGASCharacter>(GetAvatarActorFromActorInfo());
}
