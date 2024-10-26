// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "AnomalyActor.h"
#include "AnomalyDoor.generated.h"

/**
 * 
 */
UCLASS()
class ANOMALY_API AAnomalyDoor : public AAnomalyActor
{
	GENERATED_BODY()

public:
	void NativeActivateAnomaly();

protected:
	UFUNCTION(BlueprintNativeEvent)
	void ActivateAnomaly();
	UFUNCTION(BlueprintNativeEvent)
	void OnClose();

	UPROPERTY(EditAnywhere)
	USoundBase* OpenSound = nullptr;
	UPROPERTY(EditAnywhere)
	AStaticMeshActor* Door = nullptr;
	UPROPERTY(BlueprintReadOnly)
	bool bActive = false;

private:
	virtual void Tick(float DeltaSeconds) override;
	virtual void NativeClear() override;
	void OpenDoor();
	void CloseDoor();

	float DoorYaw = 0.0f;
	bool bOpening = false;
	bool bClosing = false;
	const float DoorSpeed = 30.0f;
};
