// Fill out your copyright notice in the Description page of Project Settings.


#include "AnomalySpawner.h"
#include "AnomalyGameMode.h"
#include "Engine/StaticMeshActor.h"

// Sets default values
AAnomalySpawner::AAnomalySpawner()
{
	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;
}

void AAnomalySpawner::ClearAnomalies()
{
	for (AActor* Anomaly : Anomalies)
	{
		Anomaly->Destroy();
	}
	Anomalies.Empty();
	GetWorldTimerManager().ClearTimer(SpawnTimer);
}

void AAnomalySpawner::RemoveAnomaly(AActor* Anomaly)
{
	Anomalies.Remove(Anomaly);
	Cast<AAnomalyGameMode>(GetWorld()->GetAuthGameMode())->IncrementClearedAnomalies();
}

// Called when the game starts or when spawned
void AAnomalySpawner::BeginPlay()
{
	Super::BeginPlay();

	SpawnAnomaly();
	Cast<AAnomalyGameMode>(GetWorld()->GetAuthGameMode())->RegisterSpawner(this);
}

// Called every frame
void AAnomalySpawner::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
}

void AAnomalySpawner::SpawnAnomaly()
{
	if (ValidObjects.IsEmpty() || ObjectAnomalies.IsEmpty())
	{
		return;
	}
	int ObjectIndex = FMath::RandRange(0, ValidObjects.Num() - 1);
	AStaticMeshActor* Object = ValidObjects[ObjectIndex];
	ValidObjects.RemoveAt(ObjectIndex);
	UClass* AnomalyClass = ObjectAnomalies[FMath::RandRange(0, ObjectAnomalies.Num() - 1)];
	AAnomalyActor* Anomaly = GetWorld()->SpawnActor<AAnomalyActor>(AnomalyClass, Object->GetActorLocation(),
	                                                               FRotator::ZeroRotator);
	Anomaly->SetObject(Object);
	Anomaly->OnDestroyed.AddDynamic(this, &AAnomalySpawner::RemoveAnomaly);
	Anomalies.Add(Anomaly);

	GetWorldTimerManager().SetTimer(SpawnTimer, this, &AAnomalySpawner::SpawnAnomaly,
	                                FMath::RandRange(MinSpawnTime, MaxSpawnTime));
}
