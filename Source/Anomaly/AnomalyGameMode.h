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
	void Lose();
	void Win();

private:
	virtual void BeginPlay() override;

	UPROPERTY()
	AAnomalySpawner* Spawner = nullptr;
};
