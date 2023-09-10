// Copyright Epic Games, Inc. All Rights Reserved.

#include "GASGameMode.h"
#include "GASCharacter.h"
#include "PlayerControllers/GASPlayerController.h"
#include "UObject/ConstructorHelpers.h"

AGASGameMode::AGASGameMode()
{
	// set default pawn class to our Blueprinted character
	static ConstructorHelpers::FClassFinder<APawn> PlayerPawnBPClass(TEXT("/Game/ThirdPerson/Blueprints/BP_ThirdPersonCharacter"));
	if (PlayerPawnBPClass.Class != NULL)
	{
		DefaultPawnClass = PlayerPawnBPClass.Class;
	}

	PlayerControllerClass = AGASPlayerController::StaticClass();
}

void AGASGameMode::NotifyPlayerDied(AGASPlayerController* PlayerController)
{
	if(PlayerController)
	{
		PlayerController->RestartPlayerIn(2.f);
	}
}
