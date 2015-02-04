#pragma once
#include "Projectiles/GASProjectileGlobals.h"
#include "GASBlueprintFunctionLibrary.generated.h"
/*
	Some static helper functions, to interact with Attribute system.
*/
UCLASS()
class GAMEABILITIES_API UGASBlueprintFunctionLibrary : public UBlueprintFunctionLibrary
{
	GENERATED_UCLASS_BODY()
public:
	/**
	 *	Spawn Effect Field
	 *	
	 *	@param EffectFieldClass - class of effect
	 *	@param Location - Location to where spawn this effect.
	 *	@param AbilityInstigator - Ability which spawned this effect.
	 */
	UFUNCTION(BlueprintCallable, BlueprintAuthorityOnly, Category = "Game Ability System")
		static class AGASEffectField* CreateEffectField(TSubclassOf<class AGASEffectField> EffectFieldClass, FVector Location
		, class UGASAbility* AbilityInstigator);


	UFUNCTION(BlueprintCallable, Category = "Game Ability System", meta = (UnsafeDuringActorConstruction = "true", BlueprintInternalUseOnly = "true"))
		static class AGASEffectField* BeginCreateEffectField(TSubclassOf<class AGASEffectField> EffectFieldClass
		, const FVector& Location
		, class UGASAbility* AbilityInstigator);

	UFUNCTION(BlueprintCallable, Category = "Game Ability System", meta = (UnsafeDuringActorConstruction = "true", BlueprintInternalUseOnly = "true"))
		static class AGASEffectField* FinishCreateEffectField(class AGASEffectField* EffectField);


	UFUNCTION(BlueprintCallable, Category = "Game Ability System|Projectiles", meta = (UnsafeDuringActorConstruction = "true", BlueprintInternalUseOnly = "true"))
		static class AGASProjectile* BeginSpawnProjectile(TSubclassOf<class AGASProjectile> ProjectileClass
		, const FGASProjectileConfig& Config
		, class UGASAbility* AbilityInstigator);

	UFUNCTION(BlueprintCallable, Category = "Game Ability System|Projectiles", meta = (UnsafeDuringActorConstruction = "true", BlueprintInternalUseOnly = "true"))
		static class AGASProjectile* FinishSpawnProjectile(class AGASProjectile* Projectile);
};
