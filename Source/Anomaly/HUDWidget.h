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
	void SetTime(int InSeconds);
	void BuildSummaryText(bool bWin);

private:
	virtual void NativeTick(const FGeometry& MyGeometry, float InDeltaTime) override;
	virtual void NativeConstruct() override;

	UPROPERTY(meta=(BindWidget))
	UTextBlock* Text_Time;
	UPROPERTY(meta=(BindWidget))
	UProgressBar* ProgressBar_Sanity;
	UPROPERTY(meta=(BindWidget))
	UTextBlock* Text_Summary;
	UPROPERTY(meta=(BindWidget))
	UBorder* Border_Blood;

	const float MaxSanityChange = 0.2f;
};
