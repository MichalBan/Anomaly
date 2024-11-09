// Fill out your copyright notice in the Description page of Project Settings.


#include "HUDWidget.h"
#include "AnomalyGameInstance.h"

void UHUDWidget::SetTime(int InSeconds)
{
	int Minutes = InSeconds / 60;
	int Seconds = InSeconds - (Minutes * 60);
	Text_Time->SetText(FText::FromString(FString::Printf(TEXT("%02d:%02d"), Minutes, Seconds)));
}

void UHUDWidget::BuildSummaryText(bool bWin)
{
	FString Result = bWin ? "You survived" : "Your mind succumbed to madness";
	FString Summary = FString::Printf(
		TEXT("%s\n Anomalies cleared: %d/%d"), *Result, GI->ClearedAnomalies, GI->SpawnedAnomalies);
	Text_Summary->SetText(FText::FromString(Summary));
	if (!bWin)
	{
		Border_Blood->SetVisibility(ESlateVisibility::Visible);
	}
}

void UHUDWidget::NativeTick(const FGeometry& MyGeometry, float InDeltaTime)
{
	Super::NativeTick(MyGeometry, InDeltaTime);

	float CurrentSanity = ProgressBar_Sanity->GetPercent();
	if (GI->Sanity - CurrentSanity > MaxSanityChange)
	{
		CurrentSanity += InDeltaTime * MaxSanityChange;
	}
	else if (CurrentSanity - GI->Sanity > MaxSanityChange)
	{
		CurrentSanity -= InDeltaTime * MaxSanityChange;
	}
	else
	{
		CurrentSanity = GI->Sanity;
	}
	ProgressBar_Sanity->SetPercent(CurrentSanity);
}

void UHUDWidget::NativeConstruct()
{
	Super::NativeConstruct();

	ProgressBar_Sanity->SetPercent(GI->Sanity);
}
