// Copyright Epic Games, Inc. All Rights Reserved.

#include "gioco_testGameMode.h"
#include "gioco_testCharacter.h"
#include "UObject/ConstructorHelpers.h"

Agioco_testGameMode::Agioco_testGameMode()
{
	// set default pawn class to our Blueprinted character
	static ConstructorHelpers::FClassFinder<APawn> PlayerPawnBPClass(TEXT("/Game/ThirdPersonCPP/Blueprints/ThirdPersonCharacter"));
	if (PlayerPawnBPClass.Class != NULL)
	{
		DefaultPawnClass = PlayerPawnBPClass.Class;
	}
}
