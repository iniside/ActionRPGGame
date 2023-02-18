#pragma once
#include "GAGlobalTypes.h"
#include "Effects/GAGameEffect.h"
#include "Attributes/GAAttributeGlobals.h"
#include "GAGlobalTypes.h"
#include "AFAbilityComponent.h"
#include "AFAbilityInterface.generated.h"


struct FAFAttributeBase;
struct FGAEffectHandle;
UINTERFACE(Blueprintable, meta = (CannotImplementInterfaceInBlueprint))
class ABILITYFRAMEWORK_API UAFAbilityInterface : public UInterface
{
	GENERATED_UINTERFACE_BODY()
};

class IAFAbilityInterface
{
	GENERATED_IINTERFACE_BODY()
public:
	virtual FVector GetSocketLocation(FName SocketNameIn){ return FVector::ZeroVector; };

		

	UFUNCTION(BlueprintCallable, Category = "AbilityFramework|Attributes")
		virtual class UAFAbilityComponent* GetAbilityComp() = 0;

	UFUNCTION(BlueprintCallable, Category = "AbilityFramework|Attributes")
		virtual class UAFEffectsComponent* GetEffectsComponent() = 0;

		virtual class UAFEffectsComponent* NativeGetEffectsComponent() const = 0;

	UFUNCTION(BlueprintCallable, Category = "AbilityFramework|Attributes")
		virtual float GetAttributeValue(FGAAttribute AttributeIn) const { return 0; };

	virtual void ModifyAttribute(FGAEffectMod& ModIn, const FGAEffectHandle& HandleIn,
		struct FGAEffectProperty& InProperty, const FGAEffectContext& InContext) {};
	virtual FAFAttributeBase* GetAttribute(FGAAttribute AttributeIn) { return nullptr; };
	virtual void RemoveBonus(FGAAttribute AttributeIn, const FGAEffectHandle& HandleIn, EGAAttributeMod InMod) {};

	virtual float NativeGetAttributeValue(const FGAAttribute AttributeIn) const { return 0; };

	virtual void RemoveTagContainer(const FGameplayTagContainer& TagsIn) {};
	//override to allow gathering tags from causer
	//those tags will be merged into effect owned tags.
	virtual FGameplayTagContainer GetCauserTags() { return FGameplayTagContainer(); }

	virtual FAFPredictionHandle GetPredictionHandle() { return FAFPredictionHandle(); }


	FGAEffectHandle ApplyEffectToTarget(
		const FGAEffect& EffectIn
		, const FAFEffectParams& Params
		, const FAFFunctionModifier& Modifier = FAFFunctionModifier());

	virtual class UGAAttributesBase* GetAttributes() { return GetAbilityComp()->DefaultAttributes; };

	template<class T>
	T* GetAttributesTyped()
	{
		return Cast<T>(GetAttributes());
	}
	template<class T>
	T* GetComponentTyped()
	{
		return Cast<T>(GetAbilityComp());
	}
	template<class T>
	T* GetAttributeTyped(FGAAttribute InAttribute)
	{
		return static_cast<T>(GetAttribute(InAttribute));
	}
};