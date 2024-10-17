// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "AnomalyActor.h"
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

protected:
	UPROPERTY(EditAnywhere)
	TArray<AStaticMeshActor*> ValidObjects;
	UPROPERTY(EditAnywhere)
	TArray<TSubclassOf<AAnomalyActor>> ObjectAnomalies;

private:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;
	// Called every frame
	virtual void Tick(float DeltaTime) override;
	void SpawnAnomaly();
	UFUNCTION()
	void RemoveAnomaly(AActor* Anomaly);

	UPROPERTY()
	TArray<AActor*> Anomalies;
	FTimerHandle SpawnTimer;

	const float MinSpawnTime = 10.0f;
	const float MaxSpawnTime = 20.0f;
};
