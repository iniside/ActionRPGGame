// Copyright 1998-2014 Epic Games, Inc. All Rights Reserved.

#include "GameAbilities.h"

#include "GASProjectile.h"

AGASProjectile::AGASProjectile(const FObjectInitializer& ObjectInitializer)
: Super(ObjectInitializer)
{
	bReplicates = true;
	//bReplicateInstigator = true;
	bReplicateMovement = true;
	CurrentBounces = 0;
	CollisionSphere = ObjectInitializer.CreateDefaultSubobject<USphereComponent>(this, TEXT("CollisionSphere"));
	RootComponent = CollisionSphere;

	CollisionSphere->OnComponentBeginOverlap.AddDynamic(this, &AGASProjectile::OnProjectileBeginOverlap);

	Projectile = ObjectInitializer.CreateDefaultSubobject<UProjectileMovementComponent>(this, TEXT("Projectile"));

	Projectile->OnProjectileStop.AddDynamic(this, &AGASProjectile::OnProjectileStop);
	Projectile->OnProjectileBounce.AddDynamic(this, &AGASProjectile::OnProjectileBounce);
	
	CollisionSphere->SetCollisionProfileName("ProjectileBlock");
	CollisionSphere->SetCollisionEnabled(ECollisionEnabled::NoCollision);
}


void AGASProjectile::Initialize(const FVector& ShootDirection)
{
	Projectile->bShouldBounce = ProjectileConfig.bShouldBounce;
	Projectile->bIsHomingProjectile = ProjectileConfig.bIsHomingProjectile;
	Projectile->bIsSliding = ProjectileConfig.bIsSliding;
	Projectile->Bounciness = ProjectileConfig.Bounciness;
	Projectile->ProjectileGravityScale = ProjectileConfig.GravityScale;
	Projectile->MaxSpeed = ProjectileConfig.MaximumVelocity;
	Projectile->InitialSpeed = ProjectileConfig.InitialVelocity;
	Projectile->Friction = ProjectileConfig.Friction;
	MaxBounces = ProjectileConfig.MaxBounces;
	Projectile->Velocity = ShootDirection * ProjectileConfig.InitialVelocity;

	CollisionSphere->MoveIgnoreActors.Add(Instigator);
	CollisionSphere->MoveIgnoreActors.Add(Instigator->GetOwner());
	CollisionSphere->MoveIgnoreActors.Add(Instigator->Instigator);
	CollisionSphere->SetCollisionProfileName("ProjectileBlock");

	CollisionSphere->SetCollisionEnabled(ECollisionEnabled::QueryOnly);
	if (ProjectileConfig.bExecutePeriod)
	{

	}
	FTimerDelegate del = FTimerDelegate::CreateUObject(this, &AGASProjectile::OnProjectileLifeTimeExpired);
	GetWorldTimerManager().SetTimer(ProjectileLifeTimeTimerHandle, del, ProjectileConfig.MaxLifeTime, false);
}
void AGASProjectile::OnProjectileLifeTimeExpired()
{
	//we obivously didn't hiy anything, but we still want to fire any logic we had before destroying this 
	//projectile.
	FHitResult hit;
	Projectile->Velocity = FVector::ZeroVector; //stop projectile.
	OnProjectileStop(hit);
}
void AGASProjectile::OnProjectileStop(const FHitResult& ImpactResult)
{
	OnProjectileHit(ImpactResult);

	/*
		Hide actor, and begin count to destroy iy.
	*/
	SetActorHiddenInGame(true); 
	SetLifeSpan(2.0f);
}

void AGASProjectile::OnProjectileBounce(const struct FHitResult& ImpactResult, const FVector& ImpactVelocity)
{
	//BP_OnBounced(ImpactResult);
	CurrentBounces++;
	if (CurrentBounces >= MaxBounces)
	{
		CurrentBounces = 0;
		OnProjectileStop(ImpactResult);
	}
}

void AGASProjectile::OnProjectileBeginOverlap(class AActor* OtherActor, class UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult & SweepResult)
{

}

void AGASProjectile::OnProjectileLaunched_Implementation()
{

}

void AGASProjectile::OnProjectileHit_Implementation(const FHitResult& ImpactResult)
{
	
}