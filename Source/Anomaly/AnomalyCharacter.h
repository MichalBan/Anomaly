// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "AnomalyAudioComponent.h"
#include "TP_WeaponComponent.h"
#include "Components/SpotLightComponent.h"
#include "GameFramework/Character.h"
#include "Logging/LogMacros.h"
#include "AnomalyCharacter.generated.h"

class UInputComponent;
class USkeletalMeshComponent;
class UCameraComponent;
class UInputAction;
class UInputMappingContext;
struct FInputActionValue;

DECLARE_LOG_CATEGORY_EXTERN(LogTemplateCharacter, Log, All);

UCLASS(config=Game)
class AAnomalyCharacter : public ACharacter
{
	GENERATED_BODY()

public:
	AAnomalyCharacter();

	/** Returns Mesh1P subobject **/
	USkeletalMeshComponent* GetMesh1P() const { return Mesh1P; }
	/** Returns FirstPersonCameraComponent subobject **/
	UCameraComponent* GetFirstPersonCameraComponent() const { return FirstPersonCameraComponent; }
	UFUNCTION(BlueprintCallable)
	void ChangeSanity(float DeltaSanity);
	UFUNCTION(BlueprintCallable)
	void BlockInput(bool bBlock) const;
	UFUNCTION(BlueprintCallable)
	void LowerWeapon();
	void Die();
	float GetFootstepRate();

	/** Look Input Action */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Input, meta = (AllowPrivateAccess = "true"))
	UInputAction* LookAction;

protected:
	/** Called for movement input */
	void Move(const FInputActionValue& Value);

	/** Called for looking input */
	void Look(const FInputActionValue& Value);

	virtual void BeginPlay() override;
	// APawn interface
	virtual void SetupPlayerInputComponent(UInputComponent* InputComponent) override;
	virtual void Tick(float DeltaTime) override;
	// End of APawn interface
	UPROPERTY(VisibleAnywhere, BlueprintReadWrite)
	UTP_WeaponComponent* Weapon;

private:
	/** Pawn mesh: 1st person view (arms; seen only by self) */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category=Mesh, meta = (AllowPrivateAccess = "true"))
	USkeletalMeshComponent* Mesh1P;

	/** First person camera */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Camera, meta = (AllowPrivateAccess = "true"))
	UCameraComponent* FirstPersonCameraComponent;
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Spotlight, meta = (AllowPrivateAccess = "true"))
	USpotLightComponent* Flashlight;

	/** Jump Input Action */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category=Input, meta=(AllowPrivateAccess = "true"))
	UInputAction* JumpAction;

	/** Move Input Action */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category=Input, meta=(AllowPrivateAccess = "true"))
	UInputAction* MoveAction;
	UPROPERTY(EditAnywhere)
	USoundBase* FootstepSound;

	FTimerHandle FootstepTimer;
	const float FootstepRate = 0.5f;
	const float SanityDrain = 0.007f;
};
