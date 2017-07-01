#pragma once
#include "GAEffectField.generated.h"

/*
	Base class for effect field.
	These fields might exist in two states:
	1. As long as ability is channeled.
	2. They are persistent on their own, which means that ability only create them once
	and from create tim they might exist set amount of time (including indefinetly), or 
	until they are manually removed or destroyed.

	Main use cases:
	1. Createing persistent level effects (fire wall, oil puddle, wall of ice etc).
	2. Spawning other actors (rain of meteors, other pawns).
	
	Possible functionaltity:
	1. Interacting with other fields (Wind field can extinguish fire field).
	2. Interacting with other direct damage/attribute changes (fire damage, can
	ignite oil field, creating fire field).
	Not sure about this. I would probabaly need to implement this functionality down the line in
	GameSystem Module, or add GameAttributes dependency here.

	Fields are only created on server, and then replicated back to clients.

	Need custom K2 Node to properly spawn this actor!. Default SpawnActorFromClass is
	not sufficient.
*/

UCLASS(BlueprintType, Blueprintable, DefaultToInstanced)
class ABILITYFRAMEWORK_API AGAEffectField : public AActor
{
	GENERATED_UCLASS_BODY()
public:
	/**
	 *	If false, field will exist only as long as ability is channeled.
	 *	Otherwise specific life time.
	 */
	UPROPERTY(BlueprintReadOnly, meta=(ExposeOnSpawn), Category = "Config")
		bool bIsFieldPersistent;
	
	/**
	 *	How long this field will live in world ?
	 *	Only applicable if bIsFieldPersistent = true;
	 */
	UPROPERTY(BlueprintReadOnly, meta = (ExposeOnSpawn), Category = "Config")
		float Lifetime;

	//UPROPERTY(BlueprintReadOnly, Category = "Owner")
	//class AGASAbility* AbilityInstigator;

	void InitializeField();

	void DestroyField();

	UFUNCTION(BlueprintNativeEvent, Category = "Ability Field")
		void OnAbilityExecuted();

	UFUNCTION(BlueprintNativeEvent, Category = "Ability Field")
		void OnOtherFieldOverlap();
};
