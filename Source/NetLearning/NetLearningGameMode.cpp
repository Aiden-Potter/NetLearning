// Copyright Epic Games, Inc. All Rights Reserved.

#include "NetLearningGameMode.h"
#include "NetLearningCharacter.h"
#include "UObject/ConstructorHelpers.h"

ANetLearningGameMode::ANetLearningGameMode()
{
	// set default pawn class to our Blueprinted character
	static ConstructorHelpers::FClassFinder<APawn> PlayerPawnBPClass(TEXT("/Game/ThirdPersonCPP/Blueprints/ThirdPersonCharacter"));
	if (PlayerPawnBPClass.Class != NULL)
	{
		DefaultPawnClass = PlayerPawnBPClass.Class;
	}
}
