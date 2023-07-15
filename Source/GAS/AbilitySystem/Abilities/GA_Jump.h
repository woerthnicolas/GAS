// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GASGameplayAbility.h"
#include "GA_Jump.generated.h"

class UGameplayEffect;

/**
 * 
 */
UCLASS()
class GAS_API UGA_Jump : public UGASGameplayAbility
{
	GENERATED_BODY()

	UGA_Jump();

public:
	virtual bool CanActivateAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo,
	                                const FGameplayTagContainer* SourceTags, const FGameplayTagContainer* TargetTags,
	                                FGameplayTagContainer* OptionalRelevantTags) const override;

protected:
	virtual void ActivateAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo,
	                             const FGameplayAbilityActivationInfo ActivationInfo,
	                             const FGameplayEventData* TriggerEventData) override;

	UPROPERTY(EditDefaultsOnly)
	FGameplayTag WallRunStateTag;

	UPROPERTY(EditDefaultsOnly)
	float OffWallJumpStrength = 100.f;
};
