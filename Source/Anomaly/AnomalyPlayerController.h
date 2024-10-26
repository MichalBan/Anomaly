// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "HUDWidget.h"
#include "GameFramework/PlayerController.h"
#include "AnomalyPlayerController.generated.h"

class UInputMappingContext;

/**
 *
 */
UCLASS()
class ANOMALY_API AAnomalyPlayerController : public APlayerController
{
	GENERATED_BODY()

public:
	UFUNCTION(BlueprintCallable)
	UHUDWidget* GetHUDWidget() const;

protected:
	/** Input Mapping Context to be used for player input */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Input)
	UInputMappingContext* InputMappingContext;

	virtual void BeginPlay() override;

private:
	UPROPERTY()
	UHUDWidget* HUDWidget;
	UPROPERTY(EditAnywhere)
	TSubclassOf<UHUDWidget> HUDWidgetClass;
};
