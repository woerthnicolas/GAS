// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Engine/GameInstance.h"
#include "GASGameInstance.generated.h"

/**
 * 
 */
UCLASS()
class GAS_API UGASGameInstance : public UGameInstance
{
	GENERATED_BODY()

public:
	virtual void Init() override;
};
