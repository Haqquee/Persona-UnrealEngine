// Copyright Epic Games, Inc. All Rights Reserved.

#include "NPC_Chatbot_DemoGameMode.h"
#include "NPC_Chatbot_DemoCharacter.h"
#include "UObject/ConstructorHelpers.h"

ANPC_Chatbot_DemoGameMode::ANPC_Chatbot_DemoGameMode()
	: Super()
{
	// set default pawn class to our Blueprinted character
	static ConstructorHelpers::FClassFinder<APawn> PlayerPawnClassFinder(TEXT("/Game/FirstPerson/Blueprints/BP_FirstPersonCharacter"));
	DefaultPawnClass = PlayerPawnClassFinder.Class;

}
