// Fill out your copyright notice in the Description page of Project Settings.


#include "AnomalyHUD.h"

void AAnomalyHUD::BeginPlay()
{
	Super::BeginPlay();

	HUDWidget = CreateWidget<UHUDWidget>(GetWorld(), HUDWidgetClass);
	HUDWidget->AddToViewport();
}

UHUDWidget* AAnomalyHUD::GetHUDWidget() const
{
	return HUDWidget;
}
