#pragma once
#include "GAGlobalTypes.h"
#include "GAAttributeBase.h"
#include "GAEffects.h"
#include "GAAttributesBase.generated.h"

/*
	What I need.
	Easy way to create Targeted modifications.
	For Example, we have ability Fireball, which have cast time 3s, and recharge time 5 seconds.
	Targeted modification, will only affect this one ability! 
	
	So we want  attribute which will decrease recharge time of this ability by 2 seconds (or decrease recharge time by 25%
	or increase recharge speed). How to do it ?
	
	Path of least resistance is to create special object, which will be instanced, which will listen for abilities
	which are activated, and you can cast to your particular ability (or check for tags), and just modify it's
	properties directly.

	Second way, (??) would be to create small objects, contained within array (structs), which would do the same thing.
	Except they would only be able to catch something like ability activation by tag. They wouldn't know, what kind of
	properties are on ability.

	The ability in this case would be needed to be treated as source of incoming attribute, and that object would check 
	for this attribute and modify it.
	Well kind of.

	The same problems exist for any kind of abilities more broad (like Hex), more narrow (only Necromancer Hex),
	or for weapons (we want to increase fire rate, if you have equiped machine gun).

	Though on the very base level attribute system have no idea, what kind of properties you might have on
	your machine gun.


	This bring us to the core issue. How to create system like Traits/Feats/Talents, which can contain
	myriads of possible combinations of those tree systems. We would need to mix some instanced/non-instanced UObjects
	along with plain structs. Which is probabaly going to be total mess.
*/
UCLASS(BlueprintType, Blueprintable, DefaultToInstanced, EditInlineNew)
class GAMEATTRIBUTES_API UGAAttributesBase : public UObject
{
	GENERATED_BODY()
public:
	UGAAttributesBase(const FObjectInitializer& ObjectInitializer);
	~UGAAttributesBase();

	virtual void InitializeAttributes();

	UFUNCTION(BlueprintImplementableEvent, meta = (FriendlyName = "Initialize Attributes"))
		bool BP_InitializeAttributes();
	/*
		Updates attributes.
		@param AttributeIn - attribute which changed value.

		You can use this function to update other attributes (for example derived ones),
		based on the primary attribute, if the primary attribute (AttributeIn), changed.

		I have yet to fully figure out how do I want it to work. But one thing for certain is
		that it must be fully functional from within blueprint.
	*/
	UPROPERTY()
	class UGAAttributeComponent* OwningAttributeComp;
protected:
	UProperty* FindProperty(const FGAAttribute& AttributeIn);

public:
	/*
		Ticked called from owning component.
	*/
	virtual void Tick(float DeltaTime) {};

	/*
		Central function in which we update attributes.

		It can be called from PostModifyAttribute, so I actually need to figure out if it's safe to do so.
		So we won't end up in endless loop of recursion.
	*/
	float UpdateAttributes(const FGAEvalData& AttributeIn, float newValue);

	/*
		We might want todo something on attributes, when effect is applied, to us.
		We might want to change attributes for example.

		Or we might modify effect itself.

		Usage. This function will call other function which must be named with this pattern:
		OnEffectApplied_Effect_Name.

		These function MUST be in category PostEffectApplied
	*/
	virtual void PostEffectApplied() {};
	/*
		Usage. This function will call other function which must be named with this pattern:
		OnEffectRemoved_Effect_Name.

		These function MUST be in category PostEffectRemoved
	*/
	virtual void PostEffectRemoved(const FGAEffectHandle& HandleIn, const FGAEffectSpec& SpecIn) {};
	/*
		Called before attribute will be modified.
		On both instigator (first, as an outgoing),
		and on target (second, as an incoming).

		Intentionally non const. Attribute is in and out.
	*/
	virtual FGAAttributeData PreModifyAttribute(const FGAAttributeData& AttributeMod) { return FGAAttributeData(); };
	//probabaly doesn't need to be vritual any longer. 
	/*
		Impelement your custom logic how changed attribute should affect other attributes.
	*/
	virtual float PostModifyAttribute(const FGAEvalData& AttributeMod);

	/*
		Calculate attribute mod, when it is outgoing from source.

		Implement custom logic here, which will modify attribute mods, based on what type of 
		attribute is being modified/tags/whatever.

		That's the rough idea in anycase.
	*/
	virtual void CalculateOutgoingAttributeMods(){};

	/*
		Calculate AttributeMod, when it's incoming to target.
	*/
	virtual void CalculateIncomingAttributeMods(){};

	/*
		Helper C++ functions. Shouldn't ever be exposed or called from blueprint. Also never
		try to override them.

		probabaly could also add support for int32 values.
	*/
	UStructProperty* GetStructAttribute(const FGAAttribute& Name);
	/*
		Gets pointer to compelx attribute.
	*/
	FGAAttributeBase* GetAttribute(const FGAAttribute& Name);
	/*
		Deprecated. I'm going to remove it, since it does not work as intended!
	*/
	void SetAttribute(const FGAAttribute& NameIn, UObject* NewVal);
	
	void SetAttributeAdditiveBonus(const FGAAttribute& NameIn, float NewValue);

	/*
		Gets value from complex attribute (FGAAttributeBase).
	*/
	float GetFinalAttributeValue(const FGAAttribute& Name);
	float GetCurrentAttributeValue(const FGAAttribute& Name);

	float GetFloatValue(const FGAAttribute& AttributeIn);
	float SetFloatValue(const FGAAttribute& AttributeIn, float ValueIn);
	float AttributeOperation(const FGAAttribute& AttributeIn, float ValueIn, EGAAttributeMod Operation);

	bool IsNameStableForNetworking() const override;

	bool IsSupportedForNetworking() const override
	{
		return true;
	}
	void SetNetAddressable();
protected:
	bool bNetAddressable;

private:
	UProperty* LastAttributeProp;
	FName LastAttributeName;
	// cached numeric property. WEll we probabaly don't need UProperty cache then..
	UNumericProperty* CachedFloatPropety;
	
	float AddAttributeFloat(float ValueA, float ValueB);
	float SubtractAttributeFloat(float ValueA, float ValueB);
	float MultiplyAttributeFloat(float ValueA, float ValueB);
	float DivideAttributeFloat(float ValueA, float ValueB);
};
