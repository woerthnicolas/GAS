// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/GameModeBase.h"
#include "GASGameMode.generated.h"

class AGASPlayerController;

UCLASS(minimalapi)
class AGASGameMode : public AGameModeBase
{
	GENERATED_BODY()

public:
	AGASGameMode();

	void NotifyPlayerDied(AGASPlayerController* PlayerController);
};



