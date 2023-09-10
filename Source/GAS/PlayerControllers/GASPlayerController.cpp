// Fill out your copyright notice in the Description page of Project Settings.


#include "GASPlayerController.h"

#include "AbilitySystemBlueprintLibrary.h"
#include "AbilitySystemComponent.h"
#include "GameplayTagContainer.h"
#include "GASGameMode.h"

void AGASPlayerController::RestartPlayerIn(float inTime)
{
	ChangeState(NAME_Spectating);

	GetWorld()->GetTimerManager().SetTimer(RestartPlayerTimerHandle, this, &AGASPlayerController::RestartPlayer, inTime,
	                                       false);
}

void AGASPlayerController::OnPossess(APawn* InPawn)
{
	Super::OnPossess(InPawn);

	if (UAbilitySystemComponent* AbilityComponent = UAbilitySystemBlueprintLibrary::GetAbilitySystemComponent(InPawn))
	{
		DeathStateTagDelegate = AbilityComponent->RegisterGameplayTagEvent(
			                                        FGameplayTag::RequestGameplayTag(TEXT("State.Dead")),
			                                        EGameplayTagEventType::NewOrRemoved)
		                                        .AddUObject(this, &AGASPlayerController::OnPawnDeathStateChanged);
	}
}

void AGASPlayerController::OnUnPossess()
{
	Super::OnUnPossess();

	if(DeathStateTagDelegate.IsValid())
	{
		if (UAbilitySystemComponent* AbilityComponent = UAbilitySystemBlueprintLibrary::GetAbilitySystemComponent(GetPawn()))
		{
			AbilityComponent->UnregisterGameplayTagEvent(DeathStateTagDelegate, FGameplayTag::RequestGameplayTag(TEXT("State.Dead")), EGameplayTagEventType::NewOrRemoved);
		}
	}
}

void AGASPlayerController::OnPawnDeathStateChanged(const FGameplayTag CallbackTag, int32 NewCount)
{
	if(NewCount > 0)
	{
		UWorld* World = GetWorld();

		AGASGameMode* GameMode = World ? Cast<AGASGameMode>(World->GetAuthGameMode()) : nullptr;

		if(GameMode)
		{
			GameMode->NotifyPlayerDied(this);
		}

		if(DeathStateTagDelegate.IsValid())
		{
			if (UAbilitySystemComponent* AbilityComponent = UAbilitySystemBlueprintLibrary::GetAbilitySystemComponent(GetPawn()))
			{
				AbilityComponent->UnregisterGameplayTagEvent(DeathStateTagDelegate, FGameplayTag::RequestGameplayTag(TEXT("State.Dead")), EGameplayTagEventType::NewOrRemoved);
			}
		}
	}
}

void AGASPlayerController::RestartPlayer()
{
	UWorld* World = GetWorld();

	AGASGameMode* GameMode = World ? Cast<AGASGameMode>(World->GetAuthGameMode()) : nullptr;

	if(GameMode)
	{
		GameMode->RestartPlayer(this);
	}
}
