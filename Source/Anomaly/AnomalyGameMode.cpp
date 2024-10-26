// Copyright Epic Games, Inc. All Rights Reserved.

#include "AnomalyGameMode.h"
#include "AnomalyCharacter.h"
#include "AnomalyGameInstance.h"
#include "AnomalyPlayerController.h"
#include "UObject/ConstructorHelpers.h"

AAnomalyGameMode::AAnomalyGameMode()
	: Super()
{
	// set default pawn class to our Blueprinted character
	static ConstructorHelpers::FClassFinder<APawn> PlayerPawnClassFinder(
		TEXT("/Game/FirstPerson/Blueprints/BP_FirstPersonCharacter"));
	DefaultPawnClass = PlayerPawnClassFinder.Class;
}

void AAnomalyGameMode::IncrementClearedAnomalies()
{
	++GI->ClearedAnomalies;
}

int AAnomalyGameMode::GetClearedAnomalies()
{
	return GI->ClearedAnomalies;
}

void AAnomalyGameMode::RegisterSpawner(AAnomalySpawner* InSpawner)
{
	Spawner = InSpawner;
}

void AAnomalyGameMode::BeginPlay()
{
	Super::BeginPlay();

	IConsoleVariable* CVar = IConsoleManager::Get().FindConsoleVariable(TEXT("t.MaxFPS"));
	CVar->Set(165);
	CVar = IConsoleManager::Get().FindConsoleVariable(TEXT("r.VSync"));
	CVar->Set(1);

}

void AAnomalyGameMode::Win()
{
	Cast<AAnomalyPlayerController>(GetWorld()->GetFirstPlayerController())->GetHUDWidget()->BuildSummaryText(true);
	if (Spawner)
	{
		Spawner->ClearAnomalies();
		Spawner->StopSpawning();
	}
	GI->EndGame();
}

void AAnomalyGameMode::Lose()
{
	Cast<AAnomalyPlayerController>(GetWorld()->GetFirstPlayerController())->GetHUDWidget()->BuildSummaryText(false);
	if (Spawner)
	{
		Spawner->StopSpawning();
	}
	GI->EndGame();
}
