// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "AnomalySpawner.h"
#include "GameFramework/GameModeBase.h"
#include "AnomalyGameMode.generated.h"

UCLASS(minimalapi)
class AAnomalyGameMode : public AGameModeBase
{
	GENERATED_BODY()

public:
	AAnomalyGameMode();
	void IncrementClearedAnomalies();
	int GetClearedAnomalies();
	void RegisterSpawner(AAnomalySpawner* InSpawner);

private:
	virtual void BeginPlay() override;
	void Win();
	void OnGameTimer();

	UPROPERTY()
	AAnomalySpawner* Spawner;
	FTimerHandle GameTimer;
	int TimeLeft = 0;
	int ClearedAnomalies = 0;

	const int GameTime = 70;
};



