// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "Components/SkeletalMeshComponent.h"
#include "TP_WeaponComponent.generated.h"

class AAnomalyCharacter;

UCLASS(Blueprintable, BlueprintType, ClassGroup=(Custom), meta=(BlueprintSpawnableComponent))
class ANOMALY_API UTP_WeaponComponent : public USkeletalMeshComponent
{
	GENERATED_BODY()

public:
	UPROPERTY(EditAnywhere)
	FLinearColor HeatColor = FLinearColor::White;
	/** Projectile class to spawn */
	UPROPERTY(EditDefaultsOnly, Category=Projectile)
	TSubclassOf<class AAnomalyProjectile> ProjectileClass;

	/** Sound to play each time we fire */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category=Gameplay)
	USoundBase* FireSound = nullptr;

	/** AnimMontage to play each time we fire */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Gameplay)
	UAnimMontage* FireAnimation = nullptr;

	/** Gun muzzle's offset from the characters location */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category=Gameplay)
	FVector MuzzleOffset;

	/** MappingContext */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category=Input, meta=(AllowPrivateAccess = "true"))
	class UInputMappingContext* FireMappingContext = nullptr;

	/** Fire Input Action */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category=Input, meta=(AllowPrivateAccess = "true"))
	class UInputAction* FireAction = nullptr;
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category=Input, meta=(AllowPrivateAccess = "true"))
	const UInputAction* StopFireAction = nullptr;

	/** Sets default values for this component's properties */
	UTP_WeaponComponent();

	/** Attaches the actor to a FirstPersonCharacter */
	UFUNCTION(BlueprintCallable, Category="Weapon")
	bool AttachWeapon(AAnomalyCharacter* TargetCharacter);

	/** Make the weapon Fire a Projectile */
	UFUNCTION(BlueprintCallable, Category="Weapon")
	void Fire();
	UFUNCTION()
	void StartFire();
	UFUNCTION()
	void StopFire();
	UFUNCTION(BlueprintCallable)
	void SetupHeatCylinder(UStaticMeshComponent* InHeatCylinder);
	void StopSwaying();
	void SetLowered();

protected:
	/** Ends gameplay for this component. */
	UFUNCTION()
	virtual void EndPlay(const EEndPlayReason::Type EndPlayReason) override;
	virtual void BeginPlay() override;
	virtual void TickComponent(float DeltaTime, ELevelTick TickType,
	                           FActorComponentTickFunction* ThisTickFunction) override;

private:
	FTimerHandle FireTimerHandle;
	/** The Character holding this weapon*/
	UPROPERTY()
	AAnomalyCharacter* Character = nullptr;
	UPROPERTY()
	UAudioComponent* FireAudioComponent = nullptr;
	UPROPERTY()
	UStaticMeshComponent* HeatCylinder = nullptr;
	float Heat = 0;
	bool bIsFiring = false;
	FVector BaseLocation;

	bool bLowered = false;
	bool bSnapToBase = false;
	bool bStopSwaying = false;
	float Offset = 0;
	float SwayFrequency = 1.0f;
	float Phase = 0;
	float Amplitude = 0.5f;

	const float FireRate = 0.1f;
	const float HeatingRate = 0.5f;
	const float CoolingRate = 0.2f;
	const float HeatColorFactor = 0.5f;
	const float HeatColorThreshold = 0.25f;
};
