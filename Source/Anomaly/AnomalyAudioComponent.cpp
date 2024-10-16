// Fill out your copyright notice in the Description page of Project Settings.


#include "AnomalyAudioComponent.h"

#include "Components/AudioComponent.h"
#include "Kismet/GameplayStatics.h"

// Sets default values for this component's properties
UAnomalyAudioComponent::UAnomalyAudioComponent()
{
	// Set this component to be initialized when the game starts, and to be ticked every frame.  You can turn these features
	// off to improve performance if you don't need them.
	PrimaryComponentTick.bCanEverTick = true;

	// ...
}


// Called when the game starts
void UAnomalyAudioComponent::BeginPlay()
{
	Super::BeginPlay();

	AmbientComponent = NewObject<UAudioComponent>(this, UAudioComponent::StaticClass());
	AmbientComponent->SetSound(AmbientSound);
	AmbientComponent->Play();

	GetWorld()->GetTimerManager().SetTimer(RandomTimer, this, &UAnomalyAudioComponent::PlayRandomSound,
	                                       FirstRandomDelay + FMath::RandRange(MinRandomTime, MaxRandomTime));
}

// Called every frame
void UAnomalyAudioComponent::TickComponent(float DeltaTime, ELevelTick TickType,
                                           FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

	// ...
}

void UAnomalyAudioComponent::PlayRandomSound()
{
	if (RandomSounds.IsEmpty())
	{
		return;
	}
	USoundBase* RandomSound = RandomSounds[FMath::RandRange(0, RandomSounds.Num() - 1)];
	UGameplayStatics::PlaySound2D(GetWorld(), RandomSound);
	GetWorld()->GetTimerManager().SetTimer(RandomTimer, this, &UAnomalyAudioComponent::PlayRandomSound,
	                                       FMath::RandRange(MinRandomTime, MaxRandomTime));
}