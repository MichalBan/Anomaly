// Fill out your copyright notice in the Description page of Project Settings.


#include "AnomalyGameInstance.h"

#include "AnomalyGameMode.h"
#include "AnomalyPlayerController.h"
#include "Kismet/GameplayStatics.h"

void UAnomalyGameInstance::ResetGame()
{
	Sanity = 1.0f;
	ClearedAnomalies = 0;
	SpawnedAnomalies = 0;
	TimeLeft = GameTime;
	AnomalyData.Reset();
	bGameEnded = false;
	GetWorld()->GetTimerManager().SetTimer(GameTimer, this, &UAnomalyGameInstance::OnGameTimer, 1.0f, true);
}

void UAnomalyGameInstance::EndGame()
{
	bGameEnded = true;
	GetWorld()->GetTimerManager().SetTimer(GameTimer, [this]
	{
		UGameplayStatics::OpenLevelBySoftObjectPtr(GetWorld(), MenuLevel);
	}, 5.0f, false);
}

void UAnomalyGameInstance::OnGameTimer()
{
	--GI->TimeLeft;
	Cast<AAnomalyPlayerController>(GetWorld()->GetFirstPlayerController())->GetHUDWidget()->SetTime(TimeLeft);
	if (GI->TimeLeft <= 0)
	{
		Cast<AAnomalyGameMode>(GetWorld()->GetAuthGameMode())->Win();
	}
}
