// Copyright Epic Games, Inc. All Rights Reserved.

#include "LLMTestGameMode.h"
#include "LLMTestCharacter.h"
#include "UObject/ConstructorHelpers.h"

ALLMTestGameMode::ALLMTestGameMode()
	: Super()
{
	// set default pawn class to our Blueprinted character
	static ConstructorHelpers::FClassFinder<APawn> PlayerPawnClassFinder(TEXT("/Game/FirstPerson/Blueprints/BP_FirstPersonCharacter"));
	DefaultPawnClass = PlayerPawnClassFinder.Class;

}
