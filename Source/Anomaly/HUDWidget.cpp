// Fill out your copyright notice in the Description page of Project Settings.


#include "HUDWidget.h"

void UHUDWidget::SetSanityPercent(float Percent)
{
	ProgressBar_Sanity->SetPercent(Percent);
}

void UHUDWidget::BuildSummaryText(int NumAnomalies)
{
	FString Summary = FString::Printf(TEXT("Your mind succumbed to madness\n Anomalies cleared: %d"), NumAnomalies);
	Text_Summary->SetText(FText::FromString(Summary));
}
