// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "Components/Border.h"
#include "Components/ProgressBar.h"
#include "Components/TextBlock.h"
#include "HUDWidget.generated.h"

/**
 * 
 */
UCLASS()
class ANOMALY_API UHUDWidget : public UUserWidget
{
	GENERATED_BODY()
	
public:
	void SetSanityPercent(float Percent);
	void BuildSummaryText(int NumAnomalies);

private:
	UPROPERTY(meta=(BindWidget))
	UProgressBar* ProgressBar_Sanity;
	UPROPERTY(meta=(BindWidget))
	UTextBlock* Text_Summary;
	UPROPERTY(meta=(BindWidget))
	UBorder* Border_Blood;
};
