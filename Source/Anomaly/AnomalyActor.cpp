// Fill out your copyright notice in the Description page of Project Settings.


#include "AnomalyActor.h"

#include "AnomalyCharacter.h"
#include "Components/AudioComponent.h"

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
		if (ClearSound)
		{
			AudioComponent->SetSound(ClearSound);
		}
		else
		{
			AudioComponent->Stop();
		}
		Clear();
	}
}

void AAnomalyActor::SetObject_Implementation(AStaticMeshActor* Object)
{
	// Do nothing
}

void AAnomalyActor::Clear_Implementation()
{
	Destroy();
}

// Called when the game starts or when spawned
void AAnomalyActor::BeginPlay()
{
	Super::BeginPlay();
	if (Sound)
	{
		AudioComponent = NewObject<UAudioComponent>(this, UAudioComponent::StaticClass());
		AudioComponent->SetWorldLocation(GetActorLocation() + FVector(0, 0, 100));
		AudioComponent->SetSound(Sound);
		AudioComponent->Play();
	}
}

// Called every frame
void AAnomalyActor::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
}

void AAnomalyActor::Destroyed()
{
	if (AudioComponent)
	{
		AudioComponent->Stop();
		AudioComponent->DestroyComponent();
	}
	Super::Destroyed();
}
