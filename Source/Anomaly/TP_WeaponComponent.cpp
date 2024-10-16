// Copyright Epic Games, Inc. All Rights Reserved.


#include "TP_WeaponComponent.h"
#include "AnomalyCharacter.h"
#include "AnomalyProjectile.h"
#include "GameFramework/PlayerController.h"
#include "Camera/PlayerCameraManager.h"
#include "Kismet/GameplayStatics.h"
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
	FLinearColor CurrentColor;
	DynamicMaterial->GetVectorParameterValue(TEXT("DiffuseColor"), CurrentColor);
	MaxColor = CurrentColor;
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
}

void UTP_WeaponComponent::TickComponent(float DeltaTime, ELevelTick TickType,
                                        FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

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
		Material->GetVectorParameterValue(TEXT("DiffuseColor"), CurrentColor);
		if (Heat < 0.5)
		{
			CurrentColor = MinColor;
		}
		else
		{
			float HeatColor = (Heat - 0.5) * HeatColorFactor;
			CurrentColor.R = MinColor.R + MaxColor.R * HeatColor;
			CurrentColor.G = MinColor.G + MaxColor.G * HeatColor;
			CurrentColor.B = MinColor.B + MaxColor.B * HeatColor;
		}
		Material->SetVectorParameterValue(TEXT("DiffuseColor"), CurrentColor);
	}
}
