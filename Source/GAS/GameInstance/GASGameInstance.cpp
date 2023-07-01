// Fill out your copyright notice in the Description page of Project Settings.


#include "GameInstance/GASGameInstance.h"

#include "AbilitySystemGlobals.h"

void UGASGameInstance::Init()
{
	Super::Init();

	UAbilitySystemGlobals::Get().InitGlobalData();
}
