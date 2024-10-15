// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "AnomalyActor.generated.h"

UCLASS()
class ANOMALY_API AAnomalyActor : public AActor
{
	GENERATED_BODY()
	
public:	
	AAnomalyActor();

	void TakeHit();
protected:
	UFUNCTION(BlueprintNativeEvent)
	void Clear();
	UPROPERTY(EditAnywhere)
	int Health;
	bool bClear;

	virtual void BeginPlay() override;
	virtual void Tick(float DeltaTime) override;
};
