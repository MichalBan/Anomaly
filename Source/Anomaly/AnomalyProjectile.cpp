// Copyright Epic Games, Inc. All Rights Reserved.

#include "AnomalyProjectile.h"

#include "AnomalyActor.h"
#include "GameFramework/ProjectileMovementComponent.h"
#include "Components/SphereComponent.h"

AAnomalyProjectile::AAnomalyProjectile()
{
	// Use a sphere as a simple collision representation
	CollisionComp = CreateDefaultSubobject<USphereComponent>(TEXT("SphereComp"));
	CollisionComp->InitSphereRadius(1.0f);
	CollisionComp->BodyInstance.SetCollisionProfileName("Projectile");
	CollisionComp->OnComponentHit.AddDynamic(this, &AAnomalyProjectile::OnHit);
	// set up a notification for when this component hits something blocking

	// Players can't walk on it
	CollisionComp->SetWalkableSlopeOverride(FWalkableSlopeOverride(WalkableSlope_Unwalkable, 0.f));
	CollisionComp->CanCharacterStepUpOn = ECB_No;

	// Set as root component
	RootComponent = CollisionComp;

	// Use a ProjectileMovementComponent to govern this projectile's movement
	ProjectileMovement = CreateDefaultSubobject<UProjectileMovementComponent>(TEXT("ProjectileComp"));
	ProjectileMovement->UpdatedComponent = CollisionComp;
	ProjectileMovement->InitialSpeed = 3000.f;
	ProjectileMovement->MaxSpeed = 3000.f;
	ProjectileMovement->bRotationFollowsVelocity = true;

	// Die after 3 seconds by default
	InitialLifeSpan = 3.0f;
}

void AAnomalyProjectile::OnHit(UPrimitiveComponent* HitComp, AActor* OtherActor, UPrimitiveComponent* OtherComp,
                               FVector NormalImpulse, const FHitResult& Hit)
{
	if (OtherActor && OtherActor->IsA(AAnomalyActor::StaticClass()))
	{
		Cast<AAnomalyActor>(OtherActor)->TakeHit();
	}
	else if (OtherActor && OtherActor->GetOwner() && OtherActor->GetOwner()->IsA(AAnomalyActor::StaticClass()))
	{
		Cast<AAnomalyActor>(OtherActor->GetOwner())->TakeHit();
	}
	Destroy();
}
