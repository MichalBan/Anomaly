// Copyright Epic Games, Inc. All Rights Reserved.

#include "AnomalyGameMode.h"
#include "AnomalyCharacter.h"
#include "AnomalyHUD.h"
#include "Kismet/GameplayStatics.h"
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
	++ClearedAnomalies;
}

int AAnomalyGameMode::GetClearedAnomalies()
{
	return ClearedAnomalies;
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

	TimeLeft = GameTime;
	GetWorldTimerManager().SetTimer(GameTimer, this, &AAnomalyGameMode::OnGameTimer, 1.0f, true);
}

void AAnomalyGameMode::Win()
{
	Cast<AAnomalyHUD>(GetWorld()->GetFirstPlayerController()->GetHUD())->GetHUDWidget()->BuildSummaryText(true);
	if(Spawner)
	{
		Spawner->ClearAnomalies();
	}
	GetWorldTimerManager().SetTimer(GameTimer, [this]
	{
		UGameplayStatics::OpenLevel(GetWorld(), "/Game/StarterContent/Maps/Minimal_Default");
	}, 5.0f, false);
}

void AAnomalyGameMode::OnGameTimer()
{
	--TimeLeft;
	Cast<AAnomalyHUD>(GetWorld()->GetFirstPlayerController()->GetHUD())->GetHUDWidget()->SetTime(TimeLeft);
	if (TimeLeft <= 0)
	{
		Win();
	}
}
