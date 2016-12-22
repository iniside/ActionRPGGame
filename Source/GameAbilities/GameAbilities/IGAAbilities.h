#pragma once
#include "GAGlobalTypes.h"
#include "GAGameEffect.h"
#include "GAAttributeGlobals.h"
#include "IGAAbilities.generated.h"



UINTERFACE(Blueprintable, meta = (CannotImplementInterfaceInBlueprint))
class GAMEABILITIES_API UIGAAbilities : public UInterface
{
	GENERATED_UINTERFACE_BODY()
};

class IIGAAbilities
{
	GENERATED_IINTERFACE_BODY()
public:
	virtual FVector GetSocketLocation(FName SocketNameIn){ return FVector::ZeroVector; };

	UFUNCTION(BlueprintCallable, Category = "Game Attributes")
		virtual class UGAAttributesBase* GetAttributes() = 0;

	UFUNCTION(BlueprintCallable, Category = "Game Attributes")
		virtual class UGAAbilitiesComponent* GetAbilityComp() { return nullptr; };

	UFUNCTION(BlueprintCallable, Category = "Game Attributes")
		virtual float GetAttributeValue(FGAAttribute AttributeIn) const { return 0; };

	//UFUNCTION(BlueprintPure, Category = "Game Attributes")
	//	virtual float GetAttributeValuePure(FGAAttribute AttributeIn) const { return 0; };

	virtual float NativeGetAttributeValue(const FGAAttribute AttributeIn) const { return 0; };

	//override to allow gathering tags from causer
	//those tags will be merged into effect owned tags.
	virtual FGameplayTagContainer GetCauserTags() { return FGameplayTagContainer(); }

	virtual void Died() {};
};