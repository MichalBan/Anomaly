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
	UFUNCTION(BlueprintNativeEvent)
	void SetObject(AStaticMeshActor* Object);

protected:
	virtual void BeginPlay() override;
	virtual void Tick(float DeltaTime) override;
	virtual void Destroyed() override;
	virtual void NativeClear();

	UFUNCTION(BlueprintNativeEvent)
	void Clear();

	UPROPERTY(EditAnywhere)
	int Health = 1;
	UPROPERTY(EditAnywhere)
	USoundBase* Sound = nullptr;
	UPROPERTY(EditAnywhere)
	USoundBase* ClearSound = nullptr;
	UPROPERTY(BlueprintReadWrite)
	UAudioComponent* AudioComponent = nullptr;
	UPROPERTY(BlueprintReadOnly)
	bool bClear = false;

	const float SanityReward = 0.1f;
};
