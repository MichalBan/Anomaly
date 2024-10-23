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
}

void AAnomalySpawner::StopSpawning()
{
	GetWorldTimerManager().ClearTimer(SpawnTimer);
}

void AAnomalySpawner::RemoveAnomaly(AActor* Anomaly)
{
	Anomalies.Remove(Cast<AAnomalyActor>(Anomaly));
	Cast<AAnomalyGameMode>(GetWorld()->GetAuthGameMode())->IncrementClearedAnomalies();
}

// Called when the game starts or when spawned
void AAnomalySpawner::BeginPlay()
{
	Super::BeginPlay();

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
	if (Cast<AAnomalyGameMode>(GetWorld()->GetAuthGameMode())->GetClearedAnomalies() % DoorAnomalyPeriod == 0 &&
		!Prespawned.IsEmpty())
	{
		int Index = FMath::RandRange(0, Prespawned.Num() - 1);
		Anomaly = Prespawned[Index];
		Anomaly->ActivateAnomaly();
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
			AStaticMeshActor* SpawnActor = ValidSpawns[SpawnIndex];
			FVector SpawnLocation = SpawnActor->GetActorLocation() + FVector(0, 0, 10);
			ValidSpawns.RemoveAt(SpawnIndex);
			UClass* AnomalyClass = EntityAnomalies[FMath::RandRange(0, EntityAnomalies.Num() - 1)];
			Anomaly = GetWorld()->SpawnActor<AAnomalyActor>(AnomalyClass, SpawnLocation, FRotator::ZeroRotator);
		}
		else
		{
			int ObjectIndex = FMath::RandRange(0, ValidObjects.Num() - 1);
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
}
