// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Engine/GameInstance.h"
#include "AnomalyGameInstance.generated.h"

#define GI Cast<UAnomalyGameInstance>(GetWorld()->GetGameInstance())
/**
 * 
 */
UCLASS()
class ANOMALY_API UAnomalyGameInstance : public UGameInstance
{
	GENERATED_BODY()

public:
	UFUNCTION(BlueprintCallable)
	void ResetGame();
	void EndGame();

	float Sanity = 1.0f;
	FTimerHandle GameTimer;
	int TimeLeft = 0;
	int ClearedAnomalies = 0;

private:
	void OnGameTimer();

	UPROPERTY(EditAnywhere)
	TSoftObjectPtr<UWorld> MenuLevel;
	const int GameTime = 70;
};
