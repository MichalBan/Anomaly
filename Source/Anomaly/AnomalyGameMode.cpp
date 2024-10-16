// Copyright Epic Games, Inc. All Rights Reserved.

#include "AnomalyGameMode.h"
#include "AnomalyCharacter.h"
#include "UObject/ConstructorHelpers.h"

AAnomalyGameMode::AAnomalyGameMode()
	: Super()
{
	// set default pawn class to our Blueprinted character
	static ConstructorHelpers::FClassFinder<APawn> PlayerPawnClassFinder(
		TEXT("/Game/FirstPerson/Blueprints/BP_FirstPersonCharacter"));
	DefaultPawnClass = PlayerPawnClassFinder.Class;
}

void AAnomalyGameMode::BeginPlay()
{
	Super::BeginPlay();

	IConsoleVariable* CVar = IConsoleManager::Get().FindConsoleVariable(TEXT("t.MaxFPS"));
	CVar->Set(165);
	CVar = IConsoleManager::Get().FindConsoleVariable(TEXT("r.VSync"));
	CVar->Set(1);
}
