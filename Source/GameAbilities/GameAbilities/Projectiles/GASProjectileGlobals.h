#pragma once

#include "GASProjectileGlobals.generated.h"
/*
	Could be UObject, replicated trough component.
	But is it worth fuss ?
*/

USTRUCT(BlueprintType)
struct GAMEABILITIES_API FGASProjectileConfig
{
	GENERATED_USTRUCT_BODY();
public:
	UPROPERTY(EditAnywhere, meta = (ExposeOnSpawn), Category = "Projectile Params") //possibly change to ExposeOnSpawn instead.
		float InitialVelocity;

	UPROPERTY(EditAnywhere, meta = (ExposeOnSpawn), Category = "Projectile Params") //possibly change to ExposeOnSpawn instead.
		float MaximumVelocity;

	UPROPERTY(EditAnywhere, meta = (ExposeOnSpawn), Category = "Projectile Params")
		bool bShouldBounce;

	UPROPERTY(EditAnywhere, meta = (ExposeOnSpawn), Category = "Projectile Params")
		bool bIsHomingProjectile;

	UPROPERTY(EditAnywhere, meta = (ExposeOnSpawn), Category = "Projectile Params")
		bool bIsSliding;

	UPROPERTY(EditAnywhere, meta = (ExposeOnSpawn), Category = "Projectile Params") //possibly change to ExposeOnSpawn instead.
		float GravityScale;

	UPROPERTY(EditAnywhere, meta = (ExposeOnSpawn), Category = "Projectile Params") //possibly change to ExposeOnSpawn instead.
		float Bounciness;

	UPROPERTY(EditAnywhere, meta = (ExposeOnSpawn), Category = "Projectile Params") //possibly change to ExposeOnSpawn instead.
		int32 MaxBounces;

	UPROPERTY(EditAnywhere, meta = (ExposeOnSpawn), Category = "Projectile Params") //possibly change to ExposeOnSpawn instead.
		float Friction;
	/*
		Maximum life time of projectile. After this time projectile will self destroy.
		It will apply all effects, as it would apply if projectile hit target. (assuming that projectile
		affects area, and there are approperiate actors in range).

		If 0, lifetime is infinite.
	*/
	UPROPERTY(EditAnywhere, meta = (ExposedOnSpawn), Category = "Projectile Params")
		float MaxLifeTime;
	
	/*
		If true projectile, will fire periodic event over it's life tine,
		wchich can be used to implement game logic or other things.
	*/
	UPROPERTY(EditAnywhere, meta = (ExposedOnSpawn), Category = "Projectile Params")
		bool bExecutePeriod;

	UPROPERTY(EditAnywhere, meta = (ExposedOnSpawn), Category = "Projectile Params")
		float PeriodTime;

	FGASProjectileConfig()
	{
		InitialVelocity = 0;
		MaximumVelocity = 0;
		bShouldBounce = false;
		bIsHomingProjectile = false;
		bIsSliding = false;
		GravityScale = 0;
		Bounciness = 0;
		MaxLifeTime = 0;
	}
};
