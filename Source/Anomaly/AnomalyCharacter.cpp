// Copyright Epic Games, Inc. All Rights Reserved.

#include "AnomalyCharacter.h"

#include "AnomalyHUD.h"
#include "AnomalyProjectile.h"
#include "Animation/AnimInstance.h"
#include "Camera/CameraComponent.h"
#include "Components/CapsuleComponent.h"
#include "Components/SkeletalMeshComponent.h"
#include "EnhancedInputComponent.h"
#include "EnhancedInputSubsystems.h"
#include "InputActionValue.h"
#include "Engine/LocalPlayer.h"
#include "Kismet/GameplayStatics.h"

DEFINE_LOG_CATEGORY(LogTemplateCharacter);

//////////////////////////////////////////////////////////////////////////
// AAnomalyCharacter

AAnomalyCharacter::AAnomalyCharacter()
{
	// Set size for collision capsule
	GetCapsuleComponent()->InitCapsuleSize(55.f, 96.0f);

	// Create a CameraComponent	
	FirstPersonCameraComponent = CreateDefaultSubobject<UCameraComponent>(TEXT("FirstPersonCamera"));
	FirstPersonCameraComponent->SetupAttachment(GetCapsuleComponent());
	FirstPersonCameraComponent->SetRelativeLocation(FVector(-10.f, 0.f, 60.f)); // Position the camera
	FirstPersonCameraComponent->bUsePawnControlRotation = true;

	// Create a mesh component that will be used when being viewed from a '1st person' view (when controlling this pawn)
	Mesh1P = CreateDefaultSubobject<USkeletalMeshComponent>(TEXT("CharacterMesh1P"));
	Mesh1P->SetOnlyOwnerSee(true);
	Mesh1P->SetupAttachment(FirstPersonCameraComponent);
	Mesh1P->bCastDynamicShadow = false;
	Mesh1P->CastShadow = false;
	//Mesh1P->SetRelativeRotation(FRotator(0.9f, -19.19f, 5.2f));
	Mesh1P->SetRelativeLocation(FVector(-30.f, 0.f, -150.f));
}

void AAnomalyCharacter::BeginPlay()
{
	// Call the base class  
	Super::BeginPlay();

	Sanity = 1.0f;
}

//////////////////////////////////////////////////////////////////////////// Input

void AAnomalyCharacter::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	// Set up action bindings
	if (UEnhancedInputComponent* EnhancedInputComponent = Cast<UEnhancedInputComponent>(PlayerInputComponent))
	{
		// Moving
		EnhancedInputComponent->BindAction(MoveAction, ETriggerEvent::Triggered, this, &AAnomalyCharacter::Move);

		// Looking
		EnhancedInputComponent->BindAction(LookAction, ETriggerEvent::Triggered, this, &AAnomalyCharacter::Look);
	}
	else
	{
		UE_LOG(LogTemplateCharacter, Error,
		       TEXT(
			       "'%s' Failed to find an Enhanced Input Component! This template is built to use the Enhanced Input system. If you intend to use the legacy system, then you will need to update this C++ file."
		       ), *GetNameSafe(this));
	}
}

void AAnomalyCharacter::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	if (Sanity <= 0)
	{
		AddActorLocalOffset(DeltaTime * FVector(5, 10, -30));
	}
}


void AAnomalyCharacter::ChangeSanity(float DeltaSanity)
{
	Sanity += DeltaSanity;
	AAnomalyHUD* HUD = Cast<AAnomalyHUD>(Cast<APlayerController>(GetController())->GetHUD());
	HUD->GetHUDWidget()->SetSanityPercent(Sanity);
	if (Sanity <= 0)
	{
		GetController()->InputComponent->bBlockInput = true;
		Weapon->StopFire();
		HUD->GetHUDWidget()->BuildSummaryText(ClearedAnomalies);
		FTimerHandle Timer;
		GetWorldTimerManager().SetTimer(Timer, [this]
		{
			UGameplayStatics::OpenLevel(GetWorld(), "/Game/StarterContent/Maps/Minimal_Default");
		}, 5.0f, false);
	}
}

void AAnomalyCharacter::IncrementClearedAnomalies()
{
	++ClearedAnomalies;
}

void AAnomalyCharacter::Move(const FInputActionValue& Value)
{
	// input is a Vector2D
	FVector2D MovementVector = Value.Get<FVector2D>();

	if (Controller != nullptr)
	{
		// add movement 
		AddMovementInput(GetActorForwardVector(), MovementVector.Y);
		AddMovementInput(GetActorRightVector(), MovementVector.X);

		if (!GetWorldTimerManager().IsTimerActive(FootstepTimer))
		{
			UGameplayStatics::PlaySoundAtLocation(GetWorld(), FootstepSound, GetActorLocation());
			GetWorldTimerManager().SetTimer(FootstepTimer, FootstepRate, false);
		}
	}
}

void AAnomalyCharacter::Look(const FInputActionValue& Value)
{
	// input is a Vector2D
	FVector2D LookAxisVector = Value.Get<FVector2D>() / 2;

	if (Controller != nullptr)
	{
		// add yaw and pitch input to controller
		AddControllerYawInput(LookAxisVector.X);
		AddControllerPitchInput(LookAxisVector.Y);
	}
}
