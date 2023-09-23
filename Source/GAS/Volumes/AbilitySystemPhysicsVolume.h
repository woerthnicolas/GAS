// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameplayEffectTypes.h"
#include "GameFramework/PhysicsVolume.h"
#include "AbilitySystemPhysicsVolume.generated.h"


struct FGameplayEffectSpecHandle;
struct FGameplayAbilitySpecHandle;
class UGameplayAbility;
struct FGameplayTag;
class UGameplayEffect;

USTRUCT(BlueprintType)
struct FAbilityVolumeEnteredActorInfo
{
	GENERATED_BODY()

	TArray<FGameplayAbilitySpecHandle> AppliedAbilities;

	TArray<FActiveGameplayEffectHandle> AppliedEffects;
};

UCLASS()
class GAS_API AAbilitySystemPhysicsVolume : public APhysicsVolume
{
	GENERATED_BODY()

protected:
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	TArray<TSubclassOf<UGameplayEffect>> OngoingEffectsToApply;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	TArray<TSubclassOf<UGameplayEffect>> OnExitEffectsToApply;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	bool bDrawDebug = false;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	TArray<FGameplayTag> GameplayEventsToSendOnEnter;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	TArray<FGameplayTag> GameplayEventsToSendOnExit;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	TArray<TSubclassOf<UGameplayAbility>> OngoingAbilitiesToGive;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	TArray<TSubclassOf<UGameplayAbility>> PermanentAbilitiesToGive;

	UPROPERTY()
	TMap<AActor*, FAbilityVolumeEnteredActorInfo> EnteredActorsInfoMap;

public:
	AAbilitySystemPhysicsVolume();

	virtual void ActorEnteredVolume(AActor* Other) override;

	virtual void ActorLeavingVolume(AActor* Other) override;

	virtual void Tick(float DeltaSeconds) override;
};
