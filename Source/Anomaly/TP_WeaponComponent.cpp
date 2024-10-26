// Copyright Epic Games, Inc. All Rights Reserved.


#include "TP_WeaponComponent.h"
#include "AnomalyCharacter.h"
#include "AnomalyProjectile.h"
#include "GameFramework/PlayerController.h"
#include "Camera/PlayerCameraManager.h"
#include "EnhancedInputComponent.h"
#include "EnhancedInputSubsystems.h"
#include "Animation/AnimInstance.h"
#include "Components/AudioComponent.h"
#include "Engine/LocalPlayer.h"
#include "Engine/World.h"

// Sets default values for this component's properties
UTP_WeaponComponent::UTP_WeaponComponent()
{
	// Default offset from the character location for projectiles to spawn
	MuzzleOffset = FVector(100.0f, 0.0f, 10.0f);
}


void UTP_WeaponComponent::Fire()
{
	if (Heat >= 1)
	{
		StopFire();
	}

	if (Character == nullptr || Character->GetController() == nullptr)
	{
		return;
	}

	// Try and fire a projectile
	if (ProjectileClass != nullptr)
	{
		UWorld* const World = GetWorld();
		if (World != nullptr)
		{
			APlayerController* PlayerController = Cast<APlayerController>(Character->GetController());
			FVector CameraLocation = PlayerController->PlayerCameraManager->GetCameraLocation();
			const FRotator CameraRotation = PlayerController->PlayerCameraManager->GetCameraRotation();
			FHitResult HitResult;
			GetWorld()->LineTraceSingleByChannel(HitResult, CameraLocation,
			                                     CameraLocation + CameraRotation.Vector() * 3000, ECC_Visibility);
			FVector Target = HitResult.bBlockingHit
				                 ? HitResult.ImpactPoint
				                 : CameraLocation + CameraRotation.Vector() * 3000;
			FVector SpawnLocation = GetComponentLocation() + CameraRotation.Vector() * 60;
			FRotator SpawnRotation = (Target - SpawnLocation).Rotation();
			World->SpawnActor<AAnomalyProjectile>(ProjectileClass, SpawnLocation, SpawnRotation);
		}
	}

	// Try and play a firing animation if specified
	if (FireAnimation != nullptr)
	{
		// Get the animation object for the arms mesh
		UAnimInstance* AnimInstance = Character->GetMesh1P()->GetAnimInstance();
		if (AnimInstance != nullptr)
		{
			AnimInstance->Montage_Play(FireAnimation, 1.f);
		}
	}
}

void UTP_WeaponComponent::StartFire()
{
	if (Heat > 0.8)
	{
		return;
	}
	bSnapToBase = true;
	bIsFiring = true;
	FireAudioComponent->Play();
	GetWorld()->GetTimerManager().SetTimer(FireTimerHandle, this, &UTP_WeaponComponent::Fire, FireRate, true, 0.0f);
}

void UTP_WeaponComponent::StopFire()
{
	bIsFiring = false;
	FireAudioComponent->Stop();
	GetWorld()->GetTimerManager().ClearTimer(FireTimerHandle);
}

void UTP_WeaponComponent::SetupHeatCylinder(UStaticMeshComponent* InHeatCylinder)
{
	HeatCylinder = InHeatCylinder;
	UMaterialInterface* Material = HeatCylinder->GetMaterial(0);
	UMaterialInstanceDynamic* DynamicMaterial = UMaterialInstanceDynamic::Create(Material, nullptr);
	HeatCylinder->SetMaterial(0, DynamicMaterial);
}

void UTP_WeaponComponent::StopSwaying()
{
	bStopSwaying = true;
}

bool UTP_WeaponComponent::AttachWeapon(AAnomalyCharacter* TargetCharacter)
{
	Character = TargetCharacter;

	// Check that the character is valid, and has no weapon component yet
	if (Character == nullptr || Character->GetInstanceComponents().FindItemByClass<UTP_WeaponComponent>())
	{
		return false;
	}

	//// Attach the weapon to the First Person Character
	//FAttachmentTransformRules AttachmentRules(EAttachmentRule::SnapToTarget, true);
	//AttachToComponent(Character->GetMesh1P(), AttachmentRules, FName(TEXT("GripPoint")));

	// add the weapon as an instance component to the character
	Character->AddInstanceComponent(this);

	// Set up action bindings
	if (APlayerController* PlayerController = Cast<APlayerController>(Character->GetController()))
	{
		if (UEnhancedInputLocalPlayerSubsystem* Subsystem = ULocalPlayer::GetSubsystem<
			UEnhancedInputLocalPlayerSubsystem>(PlayerController->GetLocalPlayer()))
		{
			// Set the priority of the mapping to 1, so that it overrides the Jump action with the Fire action when using touch input
			Subsystem->AddMappingContext(FireMappingContext, 1);
		}

		if (UEnhancedInputComponent* EnhancedInputComponent = Cast<UEnhancedInputComponent>(
			PlayerController->InputComponent))
		{
			// Fire
			EnhancedInputComponent->BindAction(FireAction, ETriggerEvent::Triggered, this,
			                                   &UTP_WeaponComponent::StartFire);
			EnhancedInputComponent->BindAction(StopFireAction, ETriggerEvent::Completed, this,
			                                   &UTP_WeaponComponent::StopFire);
		}
	}

	SwayFrequency = 1 / Character->GetFootstepRate();

	return true;
}

void UTP_WeaponComponent::EndPlay(const EEndPlayReason::Type EndPlayReason)
{
	if (Character == nullptr)
	{
		return;
	}

	if (APlayerController* PlayerController = Cast<APlayerController>(Character->GetController()))
	{
		if (UEnhancedInputLocalPlayerSubsystem* Subsystem = ULocalPlayer::GetSubsystem<
			UEnhancedInputLocalPlayerSubsystem>(PlayerController->GetLocalPlayer()))
		{
			Subsystem->RemoveMappingContext(FireMappingContext);
		}
	}
}

void UTP_WeaponComponent::BeginPlay()
{
	Super::BeginPlay();
	FireAudioComponent = NewObject<UAudioComponent>(this, UAudioComponent::StaticClass());
	FireAudioComponent->AttachToComponent(this, FAttachmentTransformRules::KeepRelativeTransform);
	FireAudioComponent->SetSound(FireSound);
	BaseLocation = GetRelativeLocation();
}

void UTP_WeaponComponent::TickComponent(float DeltaTime, ELevelTick TickType,
                                        FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);
	if (!Character)
	{
		return;
	}

	APlayerController* PlayerController = Cast<APlayerController>(Character->GetController());
	const FRotator CameraRotation = PlayerController->PlayerCameraManager->GetCameraRotation();
	FHitResult HitResult;
	GetWorld()->LineTraceSingleByChannel(HitResult, GetComponentLocation(),
	                                     GetComponentLocation() + CameraRotation.Vector() * 100, ECC_Visibility);
	if (HitResult.bBlockingHit)
	{
		float Angle = (100 - HitResult.Distance) * 0.8;
		SetRelativeRotation(FRotator(0, -90, Angle));
		StopFire();
	}
	else
	{
		SetRelativeRotation(FRotator(0, -90, 0));
	}

	if (bIsFiring)
	{
		Heat += DeltaTime * HeatingRate;
	}
	else if (Heat > 0)
	{
		Heat -= DeltaTime * CoolingRate;
	}

	if (Heat > 1)
	{
		Heat = 1;
	}
	else if (Heat < 0)
	{
		Heat = 0;
	}

	if (HeatCylinder)
	{
		auto Material = Cast<UMaterialInstanceDynamic>(HeatCylinder->GetMaterial(0));
		FLinearColor CurrentColor;
		Material->GetVectorParameterValue(TEXT("EmissiveColor"), CurrentColor);
		if (Heat < HeatColorThreshold)
		{
			CurrentColor = FLinearColor::Black;
		}
		else
		{
			CurrentColor.R = HeatColor.R * (Heat - HeatColorThreshold) * HeatColorFactor;
			CurrentColor.G = HeatColor.G * (Heat - HeatColorThreshold) * HeatColorFactor;
			CurrentColor.B = HeatColor.B * (Heat - HeatColorThreshold) * HeatColorFactor;
		}
		Material->SetVectorParameterValue(TEXT("EmissiveColor"), CurrentColor);
	}

	if (bSnapToBase)
	{
		float Move = DeltaTime * 2;
		if (FMath::Abs(Offset) <= FMath::Abs(Move))
		{
			bSnapToBase = false;
			Phase = 0;
			Offset = 0;
			SetRelativeLocation(BaseLocation);
		}
		else
		{
			Offset += Offset > 0 ? -Move : Move;
			SetRelativeLocation(BaseLocation + FVector(0, 0, Offset));
		}
	}
	else if (!bStopSwaying && !bIsFiring)
	{
		float Change = DeltaTime * PI * SwayFrequency;
		bool bMoving = Character->GetVelocity() != FVector::ZeroVector;
		Phase += bMoving ? Change : Change * 0.5f;
		Offset = FMath::Sin(Phase) * Amplitude;
		SetRelativeLocation(BaseLocation + FVector(0, 0, Offset));
	}
}
