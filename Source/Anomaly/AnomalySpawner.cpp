// Fill out your copyright notice in the Description page of Project Settings.


#include "AnomalySpawner.h"

#include "AnomalyGameInstance.h"
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
}

void AAnomalySpawner::StopSpawning()
{
	GetWorldTimerManager().ClearTimer(SpawnTimer);
}

void AAnomalySpawner::RemoveAnomaly(AActor* Anomaly)
{
	Anomalies.Remove(Cast<AAnomalyActor>(Anomaly));
	++GI->ClearedAnomalies;
}

// Called when the game starts or when spawned
void AAnomalySpawner::BeginPlay()
{
	Super::BeginPlay();

	for (int ObjectIndex : GI->AnomalyData.ObjectIndexes)
	{
		ValidObjects[ObjectIndex]->Destroy();
		ValidObjects.RemoveAt(ObjectIndex);
	}
	for (int SpawnIndex : GI->AnomalyData.SpawnIndexes)
	{
		ValidSpawns.RemoveAt(SpawnIndex);
	}

	for (int PrespawnedIndex : GI->AnomalyData.PrespawnedIndexes)
	{
		Prespawned.RemoveAt(PrespawnedIndex);
	}

	// we entered the level by teleporting from somewhere
	// this means we cleard a door anomaly
	if (GI->AnomalyData.PrespawnedIndexes.Num() > 0)
	{
		++GI->ClearedAnomalies;
	}

	DoorAnomalyDelay = FMath::RandRange(0, DoorAnomalyPeriod - 1);
	Cast<AAnomalyGameMode>(GetWorld()->GetAuthGameMode())->RegisterSpawner(this);
	GetWorldTimerManager().SetTimer(SpawnTimer, this, &AAnomalySpawner::SpawnAnomaly, GracePeriod);
}

// Called every frame
void AAnomalySpawner::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
}

void AAnomalySpawner::SpawnAnomaly()
{
	AAnomalyActor* Anomaly;
	if (GI->SpawnedAnomalies % DoorAnomalyPeriod == DoorAnomalyDelay && !Prespawned.IsEmpty())
	{
		int Index = FMath::RandRange(0, Prespawned.Num() - 1);
		GI->AnomalyData.PrespawnedIndexes.Add(Index);
		Anomaly = Prespawned[Index];
		Cast<AAnomalyDoor>(Anomaly)->NativeActivateAnomaly();
		Prespawned.RemoveAt(Index);
	}
	else
	{
		if (ValidObjects.IsEmpty() || ObjectAnomalies.IsEmpty())
		{
			// no more object anomalies left, only spawn entity anomalies
			EntityOdds = 1.0f;
		}

		if (ValidSpawns.IsEmpty() || EntityAnomalies.IsEmpty())
		{
			// no more entity anomalies left
			if (EntityOdds == 1.0f)
			{
				// no more anomalies of any kind left
				GEngine->AddOnScreenDebugMessage(0, 5.0f, FColor::Red, TEXT("Can not spawn anomaly"));
				StopSpawning();
			}
			EntityOdds = 0.0f;
		}

		if (FMath::RandRange(0.0f, 1.0f) < EntityOdds)
		{
			int SpawnIndex = FMath::RandRange(0, ValidSpawns.Num() - 1);
			GI->AnomalyData.SpawnIndexes.Add(SpawnIndex);
			AStaticMeshActor* SpawnActor = ValidSpawns[SpawnIndex];
			FVector SpawnLocation = SpawnActor->GetActorLocation() + FVector(0, 0, 10);
			ValidSpawns.RemoveAt(SpawnIndex);
			UClass* AnomalyClass = EntityAnomalies[FMath::RandRange(0, EntityAnomalies.Num() - 1)];
			Anomaly = GetWorld()->SpawnActor<AAnomalyActor>(AnomalyClass, SpawnLocation, FRotator::ZeroRotator);
		}
		else
		{
			int ObjectIndex = FMath::RandRange(0, ValidObjects.Num() - 1);
			GI->AnomalyData.ObjectIndexes.Add(ObjectIndex);
			AStaticMeshActor* Object = ValidObjects[ObjectIndex];
			ValidObjects.RemoveAt(ObjectIndex);

			UClass* AnomalyClass = ObjectAnomalies[FMath::RandRange(0, ObjectAnomalies.Num() - 1)];
			Anomaly = GetWorld()->SpawnActor<AAnomalyActor>(AnomalyClass, Object->GetActorLocation(),
			                                                FRotator::ZeroRotator);
			Anomaly->SetObject(Object);
		}
	}

	Anomaly->OnDestroyed.AddDynamic(this, &AAnomalySpawner::RemoveAnomaly);
	Anomalies.Add(Anomaly);
	GetWorldTimerManager().SetTimer(SpawnTimer, this, &AAnomalySpawner::SpawnAnomaly,
	                                FMath::RandRange(MinSpawnTime, MaxSpawnTime));
	++GI->SpawnedAnomalies;
}
