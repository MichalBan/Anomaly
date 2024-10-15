// Fill out your copyright notice in the Description page of Project Settings.


#include "AnomalyActor.h"

// Sets default values
AAnomalyActor::AAnomalyActor()
{
	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;
}

void AAnomalyActor::TakeHit()
{
	if (bClear)
	{
		return;
	}

	--Health;
	if (Health <= 0)
	{
		bClear = true;
		Clear();
	}
}

void AAnomalyActor::Clear_Implementation()
{
	Destroy();
}

// Called when the game starts or when spawned
void AAnomalyActor::BeginPlay()
{
	Super::BeginPlay();
}

// Called every frame
void AAnomalyActor::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
}