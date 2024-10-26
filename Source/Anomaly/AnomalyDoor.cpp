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

	if (!bOpening && !bClosing)
	{
		return;
	}

	float AngleChange = DoorSpeed * DeltaSeconds;
	Door->AddActorLocalRotation(FRotator(0, bOpening ? AngleChange : -AngleChange, 0));
	DoorYaw += AngleChange;
	if (DoorYaw >= 90.0f)
	{
		Door->AddActorLocalRotation(FRotator(0, bOpening ? 90.0f - DoorYaw : DoorYaw - 90.0f, 0));
		if (bClosing)
		{
			bClosing = false;
			OnClose();
			Destroy();
		}
		else
		{
			bOpening = false;
		}
	}
}

void AAnomalyDoor::NativeClear()
{
	Super::NativeClear();

	CloseDoor();
}

void AAnomalyDoor::OpenDoor()
{
	bOpening = true;
	DoorYaw = 0.0f;
	UGameplayStatics::PlaySoundAtLocation(GetWorld(), OpenSound, GetActorLocation());
}

void AAnomalyDoor::CloseDoor()
{
	bClosing = true;
	DoorYaw = 0.0f;
}
