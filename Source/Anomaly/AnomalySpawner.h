// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "AnomalyActor.h"
#include "AnomalyDoor.h"
#include "GameFramework/Actor.h"
#include "AnomalySpawner.generated.h"

UCLASS()
class ANOMALY_API AAnomalySpawner : public AActor
{
	GENERATED_BODY()

public:
	// Sets default values for this actor's properties
	AAnomalySpawner();
	void ClearAnomalies();
	void StopSpawning();

protected:
	UPROPERTY(EditAnywhere)
	TArray<AStaticMeshActor*> ValidObjects;
	UPROPERTY(EditAnywhere)
	TArray<TSubclassOf<AAnomalyActor>> ObjectAnomalies;

	UPROPERTY(EditAnywhere)
	TArray<AStaticMeshActor*> ValidSpawns;
	UPROPERTY(EditAnywhere)
	TArray<TSubclassOf<AAnomalyActor>> EntityAnomalies;

	UPROPERTY(EditAnywhere)
	TArray<AAnomalyDoor*> Prespawned;

private:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;
	// Called every frame
	virtual void Tick(float DeltaTime) override;
	void SpawnAnomaly();
	UFUNCTION()
	void RemoveAnomaly(AActor* Anomaly);

	UPROPERTY()
	TArray<AAnomalyActor*> Anomalies;
	FTimerHandle SpawnTimer;

	float EntityOdds = 0.2f;
	int DoorAnomalyDelay = 0;
	const int DoorAnomalyPeriod = 5;
	const float MinSpawnTime = 10.0f;
	const float MaxSpawnTime = 20.0f;
	const float GracePeriod = 5.0f;
};
