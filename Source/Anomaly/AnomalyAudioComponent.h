// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "AnomalyAudioComponent.generated.h"


UCLASS(ClassGroup=(Custom), meta=(BlueprintSpawnableComponent), Blueprintable, BlueprintType)
class ANOMALY_API UAnomalyAudioComponent : public UActorComponent
{
	GENERATED_BODY()

public:
	// Sets default values for this component's properties
	UAnomalyAudioComponent();
	UFUNCTION(BlueprintCallable)
	void SetAmbientSound(USoundBase* InSound);

protected:
	// Called when the game starts
	virtual void BeginPlay() override;

private:
	UFUNCTION()
	void PlayRandomSound();

	UPROPERTY(EditAnywhere)
	USoundBase* AmbientSound = nullptr;
	UPROPERTY(EditAnywhere)
	TArray<USoundBase*> RandomSounds;

	UPROPERTY()
	UAudioComponent* AmbientComponent = nullptr;
	FTimerHandle RandomTimer;
	const float FirstRandomDelay = 10.0f;
	const float MaxRandomTime = 15.0f;
	const float MinRandomTime = 3.0f;
};
