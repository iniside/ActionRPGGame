// Copyright 1998-2014 Epic Games, Inc. All Rights Reserved.

#include "GameSystem.h"

#include "GSProjectile.h"

AGSProjectile::AGSProjectile(const FObjectInitializer& ObjectInitializer)
: Super(ObjectInitializer)
{
	bReplicates = true;
	//bReplicateInstigator = true;
	bReplicateMovement = true;
	CollisionSphere = ObjectInitializer.CreateDefaultSubobject<USphereComponent>(this, TEXT("CollisionSphere"));
	RootComponent = CollisionSphere;

	CollisionSphere->OnComponentBeginOverlap.AddDynamic(this, &AGSProjectile::OnProjectileBeginOverlap);

	Projectile = ObjectInitializer.CreateDefaultSubobject<UProjectileMovementComponent>(this, TEXT("Projectile"));

	Projectile->OnProjectileStop.AddDynamic(this, &AGSProjectile::OnProjectileStop);
	Projectile->OnProjectileBounce.AddDynamic(this, &AGSProjectile::OnProjectileBounce);
	
	CollisionSphere->SetCollisionProfileName("ProjectileBlock");
	CollisionSphere->SetCollisionEnabled(ECollisionEnabled::NoCollision);
}


void AGSProjectile::Initialize(const FVector& ShootDirection)
{
	Projectile->Velocity = ShootDirection * InitialVelocity;

	CollisionSphere->MoveIgnoreActors.Add(Instigator);
	CollisionSphere->MoveIgnoreActors.Add(Instigator->GetOwner());
	CollisionSphere->MoveIgnoreActors.Add(Instigator->Instigator);
	CollisionSphere->SetCollisionProfileName("ProjectileBlock");

	CollisionSphere->SetCollisionEnabled(ECollisionEnabled::QueryOnly);
}

void AGSProjectile::OnProjectileStop(const FHitResult& ImpactResult)
{
	OnProjectileHit(ImpactResult);
}

void AGSProjectile::OnProjectileBounce(const struct FHitResult& ImpactResult, const FVector& ImpactVelocity)
{

}

void AGSProjectile::OnProjectileBeginOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResul)
{

}

void AGSProjectile::OnProjectileLaunched_Implementation()
{

}

void AGSProjectile::OnProjectileHit_Implementation(const FHitResult& ImpactResult)
{
	
}