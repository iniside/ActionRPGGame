#pragma once
#include "GAGlobalTypes.h"
#include "Effects/GAGameEffect.h"
#include "Attributes/GAAttributeGlobals.h"
#include "GAGlobalTypes.h"
//#include "Messaging.h"
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
		virtual class UGAAttributesBase* GetAttributes() = 0;

	UFUNCTION(BlueprintCallable, Category = "AbilityFramework|Attributes")
		virtual class UAFAbilityComponent* GetAbilityComp() { return nullptr; };

	UFUNCTION(BlueprintCallable, Category = "AbilityFramework|Attributes")
		virtual float GetAttributeValue(FGAAttribute AttributeIn) const { return 0; };

	virtual void ModifyAttribute(FGAEffectMod& ModIn, const FGAEffectHandle& HandleIn,
		struct FGAEffectProperty& InProperty) {};
	virtual FAFAttributeBase* GetAttribute(FGAAttribute AttributeIn) { return nullptr; };
	virtual void RemoveBonus(FGAAttribute AttributeIn, const FGAEffectHandle& HandleIn, EGAAttributeMod InMod) {};

	virtual float NativeGetAttributeValue(const FGAAttribute AttributeIn) const { return 0; };

	virtual FGAEffectHandle ApplyEffectToTarget(FGAEffect* EffectIn,
		FGAEffectProperty& InProperty, FGAEffectContext& InContext) { return FGAEffectHandle(); };
	virtual void RemoveTagContainer(const FGameplayTagContainer& TagsIn) {};
	//override to allow gathering tags from causer
	//those tags will be merged into effect owned tags.
	virtual FGameplayTagContainer GetCauserTags() { return FGameplayTagContainer(); }

	virtual FAFPredictionHandle GetPredictionHandle() { return FAFPredictionHandle(); }

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