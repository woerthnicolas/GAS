// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Abilities/Tasks/AbilityTask.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "AbilityTask_TickWallRun.generated.h"

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnWallRunWallSideDeterminedDelegate, bool, bLeftSide);

DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnWallRunFinishedDelegate);

class ACharacter;
class UCharacterMovementComponent;

/**
 * 
 */
UCLASS()
class GAS_API UAbilityTask_TickWallRun : public UAbilityTask
{
	GENERATED_BODY()

public:
	UPROPERTY(BlueprintAssignable)
	FOnWallRunFinishedDelegate OnFinished;

	UPROPERTY(BlueprintAssignable)
	FOnWallRunWallSideDeterminedDelegate OnWallRunWallSideDetermined;

	UFUNCTION(BlueprintCallable, Category = "Ability|Tasks", meta = (HiddenPin = "OwningAbility", DefaultToSelf = "OwningAbility"))
	static UAbilityTask_TickWallRun* CreateWallRunTask(
		UGameplayAbility* OwningAbility,
		ACharacter* InCharacter,
		UCharacterMovementComponent* InCharacterMovement,
		TArray<TEnumAsByte<EObjectTypeQuery>> TraceObjectTypes
		);

protected:
	virtual void Activate() override;

	UPROPERTY()
	UCharacterMovementComponent* CharacterMovement = nullptr;

	UPROPERTY()
	ACharacter* CharacterOwner = nullptr;

	TArray<TEnumAsByte<EObjectTypeQuery>> WallRun_TraceObjectTypes;

	bool FindRunnableWall(FHitResult& OnWallHit);

	bool IsWallOnTheLeft(const FHitResult& InWallHit) const;

public:
	virtual void TickTask(float DeltaTime) override;
	virtual void OnDestroy(bool bInOwnerFinished) override;
};
