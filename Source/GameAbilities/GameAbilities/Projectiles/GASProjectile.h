#pragma once
#include "GASProjectileGlobals.h"
#include "GASProjectile.generated.h"


UCLASS(BlueprintType, Blueprintable)
class GAMEABILITIES_API AGASProjectile : public AActor
{
	GENERATED_BODY()
public:
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Projectile")
		UProjectileMovementComponent* Projectile;
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Projectile")
		USphereComponent* CollisionSphere;

public:
	AGASProjectile(const FObjectInitializer& ObjectInitializer);

	UPROPERTY(BlueprintReadWrite, Category = "Projectile Params")
		FGASProjectileConfig ProjectileConfig;
protected:
	FTimerHandle ProjectileLifeTimeTimerHandle;
	FTimerHandle ProjectileIntervalTimerHandle;

	int32 MaxBounces;

	int32 CurrentBounces;
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

	UFUNCTION(BlueprintImplementableEvent)
		void OnPeriod();

	UFUNCTION(BlueprintNativeEvent)
		void OnProjectileLaunched();
	virtual void OnProjectileLaunched_Implementation(); 

	UFUNCTION(BlueprintNativeEvent)
		void OnProjectileHit(const FHitResult& ImpactResult);
	virtual void OnProjectileHit_Implementation(const FHitResult& ImpactResult);

	//UFUNCTION(BlueprintImpelementableEvet, meta=(FriendlyName="On Bounce"))
	//	void BP_OnBounced(const FHitResult& Impact);

	UFUNCTION()
		void OnProjectileLifeTimeExpired();
};
