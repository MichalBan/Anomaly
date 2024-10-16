// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "HUDWidget.h"
#include "GameFramework/HUD.h"
#include "AnomalyHUD.generated.h"

/**
 * 
 */
UCLASS()
class ANOMALY_API AAnomalyHUD : public AHUD
{
	GENERATED_BODY()

public:
	virtual void BeginPlay() override;
	UHUDWidget* GetHUDWidget() const;

private:
	UPROPERTY()
	UHUDWidget* HUDWidget;
	UPROPERTY(EditAnywhere)
	TSubclassOf<UHUDWidget> HUDWidgetClass;
	
};
