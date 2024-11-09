// Fill out your copyright notice in the Description page of Project Settings.


#include "AnomalyDoor.h"

#include "Engine/StaticMeshActor.h"
#include "Kismet/GameplayStatics.h"

void AAnomalyDoor::OnClose_Implementation()
{
	// do nothing
}

void AAnomalyDoor::ActivateAnomaly_Implementation()
{
	// do nothing
}

void AAnomalyDoor::NativeActivateAnomaly()
{
	bActive = true;
	ActivateAnomaly();
	OpenDoor();
}

void AAnomalyDoor::Tick(float DeltaSeconds)
{
	Super::Tick(DeltaSeconds);

	switch (DoorState)
	{
	case EDoorState::Closed:
		break;
	case EDoorState::Open:
		break;
	case EDoorState::Opening:
		Door->AddActorLocalRotation(FRotator(0, DoorSpeed * DeltaSeconds, 0));
		DoorYaw += DoorSpeed * DeltaSeconds;
		GEngine->AddOnScreenDebugMessage(1, 1.0f, FColor::Red, FString::Printf(TEXT("DoorYaw: %f"), DoorYaw));
		if (DoorYaw > 90.0f)
		{
			Door->AddActorLocalRotation(FRotator(0, 90.0f - DoorYaw, 0));
			DoorState = EDoorState::Open;
		}
		break;
	case EDoorState::Closing:
		Door->AddActorLocalRotation(FRotator(0, -DoorSpeed * DeltaSeconds, 0));
		DoorYaw -= DoorSpeed * DeltaSeconds;
		GEngine->AddOnScreenDebugMessage(1, 1.0f, FColor::Red, FString::Printf(TEXT("DoorYaw: %f"), DoorYaw));
		if (DoorYaw < 0.0f)
		{
			Door->AddActorLocalRotation(FRotator(0, -DoorYaw, 0));
			DoorState = EDoorState::Closed;
			OnClose();
			Destroy();
		}
		break;
	}
}

void AAnomalyDoor::NativeClear()
{
	Super::NativeClear();
	DoorState = EDoorState::Closing;
}

void AAnomalyDoor::OpenDoor()
{
	DoorState = EDoorState::Opening;
	UGameplayStatics::PlaySoundAtLocation(GetWorld(), OpenSound, Door->GetActorLocation());
}
