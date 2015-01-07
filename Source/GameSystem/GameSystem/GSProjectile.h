#pragma once

#include "GSProjectile.generated.h"
/*
	Could be UObject, replicated trough component.
	But is it worth fuss ?
*/

USTRUCT(BlueprintType)
struct FGSProjectileConfig
{
	GENERATED_USTRUCT_BODY();
public:
	UPROPERTY(EditAnywhere, Category = "Projectile Params") //possibly change to ExposeOnSpawn instead.
		float InitialVelocity;

	UPROPERTY(EditAnywhere, Category = "Projectile Params") //possibly change to ExposeOnSpawn instead.
		float MaximumVelocity;

	UPROPERTY(EditAnywhere, Category = "Projectile Params")
		bool bShouldBounce;

	UPROPERTY(EditAnywhere, Category = "Projectile Params")
		bool bIsHomingProjectile;

	UPROPERTY(EditAnywhere, Category = "Projectile Params")
		bool bIsSliding;

	UPROPERTY(EditAnywhere, Category = "Projectile Params") //possibly change to ExposeOnSpawn instead.
		float GravityScale;

	UPROPERTY(EditAnywhere, Category = "Projectile Params") //possibly change to ExposeOnSpawn instead.
		float Bounciness;
};

UCLASS(BlueprintType, Blueprintable)
class GAMESYSTEM_API AGSProjectile : public AActor
{
	GENERATED_UCLASS_BODY()
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Projectile")
		UProjectileMovementComponent* Projectile;
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Projectile")
		USphereComponent* CollisionSphere;

public:
	UPROPERTY(BlueprintReadWrite, meta=(ExposeOnSpawn), Category = "Projectile Params") //possibly change to ExposeOnSpawn instead.
		float InitialVelocity;

	UPROPERTY(EditAnywhere, Category = "Projectile Params") //possibly change to ExposeOnSpawn instead.
		float MaximumVelocity;

	UPROPERTY(EditAnywhere, Category = "Projectile Params")
		bool bShouldBounce;

	UPROPERTY(EditAnywhere, Category = "Projectile Params")
		bool bIsHomingProjectile;

	UPROPERTY(EditAnywhere, Category = "Projectile Params")
		bool bIsSliding;

	UPROPERTY(EditAnywhere, Category = "Projectile Params") //possibly change to ExposeOnSpawn instead.
		float GravityScale;

	UPROPERTY(EditAnywhere, Category = "Projectile Params") //possibly change to ExposeOnSpawn instead.
		float Bounciness;

	UPROPERTY(BlueprintReadWrite, meta = (ExposeOnSpawn), Category = "Projectile Params")
		FGSProjectileConfig ProjectileConfig;
public:
	/*
		will need that to:
		1. Set collision (by default spawned without collision, to
		avoid colliding with pawn which spawned this projectile).
		2. Set list of ignored actors.
		3. Set collision on.
		4. Launch projectile.

		Called After BeginPlay();
	*/
	UFUNCTION(BlueprintCallable, Category = "Game System|Projectile") //temp for testing in BP
	virtual void Initialize(const FVector& ShootDirection);

	UFUNCTION()
		void OnProjectileStop(const FHitResult& ImpactResult);

	UFUNCTION()
		void OnProjectileBounce(const struct FHitResult& ImpactResult, const FVector& ImpactVelocity);

	UFUNCTION()
		void OnProjectileBeginOverlap(class AActor* OtherActor, class UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult & SweepResult);


	UFUNCTION(BlueprintNativeEvent)
		void OnProjectileLaunched();

	UFUNCTION(BlueprintNativeEvent)
		void OnProjectileHit(const FHitResult& ImpactResult);
};
