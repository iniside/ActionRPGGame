// Copyright 1998-2014 Epic Games, Inc. All Rights Reserved.

#include "GameAbilities.h"

#include "GASAbility.h"
#include "GASEffectField.h"
#include "Projectiles/GASProjectile.h"
#include "Tracing/GASTrace.h"
#include "GASBlueprintFunctionLibrary.h"



UGASBlueprintFunctionLibrary::UGASBlueprintFunctionLibrary(const FObjectInitializer& ObjectInitializer)
: Super(ObjectInitializer)
{

}

class AGASEffectField* UGASBlueprintFunctionLibrary::CreateEffectField(TSubclassOf<class AGASEffectField> EffectFieldClass, FVector Location
	, class UGASAbility* AbilityInstigator)
{
	return nullptr;
}

class AGASEffectField* UGASBlueprintFunctionLibrary::BeginCreateEffectField(TSubclassOf<class AGASEffectField> EffectFieldClass
	, const FVector& Location
	, class UGASAbility* AbilityInstigator)
{
	AGASEffectField* effectField = nullptr;

	//if (AbilityInstigator)
	//{
	//	FRotator Rotation = FRotator::ZeroRotator;
	//	Rotation = AbilityInstigator->GetPawn()->GetBaseAimRotation();
	//	Rotation.Pitch = 0;
	//	effectField = AbilityInstigator->GetWorld()->SpawnActorDeferred<AGASEffectField>(EffectFieldClass,
	//		Location, Rotation, AbilityInstigator, AbilityInstigator->GetPawn(), true);
	//	if (effectField)
	//	{
	//		effectField->AbilityInstigator = AbilityInstigator;
	//	}
	//}

	return effectField;
}


class AGASEffectField* UGASBlueprintFunctionLibrary::FinishCreateEffectField(class AGASEffectField* EffectField)
{
	
	if (EffectField)
	{
		//FTransform Trans;
		//EffectField->FinishSpawning(Trans);
		//EffectField->InitializeField();
	}
	return EffectField;
}

class AGASProjectile* UGASBlueprintFunctionLibrary::BeginSpawnProjectile(TSubclassOf<class AGASProjectile> ProjectileClass
, const FGASProjectileConfig& Config
, class UGASAbility* AbilityInstigator)
{
	AGASProjectile* Proj = nullptr;
	if (!ProjectileClass && !AbilityInstigator)
		return Proj;
	if (!AbilityInstigator->GetWorld())
		return Proj;
	UWorld* world = AbilityInstigator->GetWorld();
	
	FRotator Rot = FRotator::ZeroRotator; // ShootDir.Rotation();
	FVector hit = AbilityInstigator->Targeting->GetSingHitLocation();
	
	FVector SpawnLocation = AbilityInstigator->GetSocketLocation(" ");
	FVector ShootDir = (hit - SpawnLocation).GetSafeNormal();

	Proj = world->SpawnActorDeferred<AGASProjectile>(ProjectileClass, SpawnLocation,
	Rot, AbilityInstigator->POwner, AbilityInstigator->POwner, false);

	
	if (Proj)
	{
		Proj->ProjectileConfig = Config;
		Proj->Initialize(ShootDir);
	}

	return Proj;
}


class AGASProjectile* UGASBlueprintFunctionLibrary::FinishSpawnProjectile(class AGASProjectile* Projectile)
{
	if (Projectile)
	{
		FTransform Trans;// = Projectile->GetTransform();;
		Trans.SetLocation(Projectile->GetActorLocation());
		Projectile->FinishSpawning(Trans);
	}
	return Projectile;
}