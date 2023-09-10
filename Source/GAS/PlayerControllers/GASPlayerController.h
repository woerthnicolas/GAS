// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/PlayerController.h"
#include "Abilities//GameplayAbility.h"
#include "GASPlayerController.generated.h"

UCLASS()
class GAS_API AGASPlayerController : public APlayerController
{
	GENERATED_BODY()

public:

	void RestartPlayerIn(float inTime);

protected:
	virtual void OnPossess(APawn* InPawn) override;

	virtual void OnUnPossess() override;

	UFUNCTION()
	void OnPawnDeathStateChanged(const FGameplayTag CallbackTag, int32 NewCount);

	void RestartPlayer();

	FTimerHandle RestartPlayerTimerHandle;

	FDelegateHandle DeathStateTagDelegate;
};
