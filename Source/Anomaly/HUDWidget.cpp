// Fill out your copyright notice in the Description page of Project Settings.


#include "HUDWidget.h"

#include "AnomalyGameMode.h"

void UHUDWidget::SetSanityPercent(float Percent)
{
	GoalSanity = Percent;
}

void UHUDWidget::SetTime(int InSeconds)
{
	int Minutes = InSeconds / 60;
	int Seconds = InSeconds - (Minutes * 60);
	Text_Time->SetText(FText::FromString(FString::Printf(TEXT("%02d:%02d"), Minutes, Seconds)));
}

void UHUDWidget::BuildSummaryText(bool bWin)
{
	int NumAnomalies = Cast<AAnomalyGameMode>(GetWorld()->GetAuthGameMode())->GetClearedAnomalies();
	FString Result = bWin ? "You survived" : "Your mind succumbed to madness";
	FString Summary = FString::Printf(TEXT("%s\n Anomalies cleared: %d"), *Result, NumAnomalies);
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
	if (GoalSanity - CurrentSanity > MaxSanityChange)
	{
		CurrentSanity += InDeltaTime * MaxSanityChange;
	}
	else if (CurrentSanity - GoalSanity > MaxSanityChange)
	{
		CurrentSanity -= InDeltaTime * MaxSanityChange;
	}
	else
	{
		CurrentSanity = GoalSanity;
	}
	ProgressBar_Sanity->SetPercent(CurrentSanity);
}
