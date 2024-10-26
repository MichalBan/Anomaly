// Copyright Epic Games, Inc. All Rights Reserved.

#include "AnomalyCharacter.h"

#include "AnomalyGameMode.h"
#include "AnomalyPlayerController.h"
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

	Flashlight = CreateDefaultSubobject<USpotLightComponent>(TEXT("Flashlight"));
	Flashlight->SetupAttachment(FirstPersonCameraComponent);
	Flashlight->SetOuterConeAngle(20);
	Flashlight->SetRelativeLocation(FVector(0, 0, 0));
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
		AddActorLocalOffset(DeltaTime * FVector(5, 10, -25));
		if (Weapon)
		{
			Weapon->AddRelativeLocation(DeltaTime * FVector(-10, -3, -5));
		}
	}
	else
	{
		ChangeSanity(-DeltaTime * SanityDrain);
	}
}


void AAnomalyCharacter::Die()
{
	BlockInput(true);
	if (Weapon)
	{
		Weapon->StopFire();
		Weapon->StopSwaying();
	}
	if (Flashlight)
	{
		Flashlight->DestroyComponent();
	}
	SetActorEnableCollision(false);
	GetCapsuleComponent()->SetEnableGravity(false);
}

void AAnomalyCharacter::ChangeSanity(float DeltaSanity)
{
	Sanity += DeltaSanity;
	if (Sanity > 1.0f)
	{
		Sanity = 1.0f;
	}
	else if (Sanity <= 0)
	{
		Sanity = 0;
		Die();
		Cast<AAnomalyGameMode>(GetWorld()->GetAuthGameMode())->Lose();
	}
	Cast<AAnomalyPlayerController>(GetWorld()->GetFirstPlayerController())->GetHUDWidget()->SetSanityPercent(Sanity);
}

void AAnomalyCharacter::BlockInput(bool bBlock) const
{
	GetController()->InputComponent->bBlockInput = bBlock;
}

float AAnomalyCharacter::GetFootstepRate()
{
	return FootstepRate;
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
