#pragma once
#include "CoreMinimal.h"
#include "UObject/ObjectMacros.h"
#include "UObject/Object.h"
#include "UObject/Class.h"
#include "Templates/SubclassOf.h"
#include "UObject/UnrealType.h"
#include "UObject/CoreNet.h"
#include "../Effects/GAGameEffect.h"
#include "../GAGlobalTypes.h"
#include "GAAttributeBase.h"
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
class ABILITYFRAMEWORK_API UGAAttributesBase : public UObject
{
	GENERATED_BODY()
public:

	UPROPERTY(EditAnywhere, BlueprintReadOnly, meta=(ExposeOnSpawn))
		UDataTable* AttributeValues;
	UGAAttributesBase(const FObjectInitializer& ObjectInitializer);
	~UGAAttributesBase();
	//virtual void PostNetReceive() override;
	virtual void InitializeAttributes(UAFAbilityComponent* InOwningAttributeComp);
	void CopyFromOtherAttributes(UGAAttributesBase* Other);

	/*
		Copy attributes from arbitrary struct.
		Struct must be composed from FAFAttributeBase (or it's derivative) fields.
	*/
	void CopyFromStruct(UStruct* StructType, void* StructObject);
	void InitializeAttributesFromTable();
	UFUNCTION(BlueprintImplementableEvent, meta = (DisplayName = "Initialize Attributes"))
		bool BP_InitializeAttributes();
	/*
		Updates attributes.
		@param AttributeIn - attribute which changed value.

		You can use this function to update other attributes (for example derived ones),
		based on the primary attribute, if the primary attribute (AttributeIn), changed.

		I have yet to fully figure out how do I want it to work. But one thing for certain is
		that it must be fully functional from within blueprint.
	*/
	UPROPERTY(Replicated)
	class UAFAbilityComponent* OwningAttributeComp;
protected:
	UProperty* FindProperty(const FGAAttribute& AttributeIn);

public:
	/*
		Ticked called from owning component.
	*/
	virtual void Tick(float DeltaTime);// {};
	/*
		Helper C++ functions. Shouldn't ever be exposed or called from blueprint. Also never
		try to override them.

		probabaly could also add support for int32 values.
	*/
	UStructProperty* GetStructAttribute(const FGAAttribute& Name);
	/*
		Gets pointer to compelx attribute.
	*/
	FAFAttributeBase* GetAttribute(const FGAAttribute& Name);
	/*
		Deprecated. I'm going to remove it, since it does not work as intended!
	*/
	void SetAttribute(const FGAAttribute& NameIn, UObject* NewVal);
	
	void SetAttributeAdditiveBonus(const FGAAttribute& NameIn, float NewValue);

	/*
		Gets value from complex attribute (FAFAttributeBase).
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

	void ModifyAttribute(const FGAEffect& EffectIn);
	float ModifyAttribute(const FGAEffectMod& ModIn
		, const FGAEffectHandle& HandleIn
		, FGAEffectProperty& InProperty
		, const FGAEffectContext& InContext);
	void RemoveBonus(FGAAttribute AttributeIn, const FGAEffectHandle& HandleIn, EGAAttributeMod InMod);
protected:
	bool bNetAddressable;

private:
	TArray<FAFAttributeBase*> TickableAttributes;
	UProperty* LastAttributeProp;
	FName LastAttributeName;
	// cached numeric property. WEll we probabaly don't need UProperty cache then..
	UNumericProperty* CachedFloatPropety;
	
	float AddAttributeFloat(float ValueA, float ValueB);
	float SubtractAttributeFloat(float ValueA, float ValueB);
	float MultiplyAttributeFloat(float ValueA, float ValueB);
	float DivideAttributeFloat(float ValueA, float ValueB);

	void OnAttributeModified(const FGAEffectMod& InMod, const FGAEffectHandle& InHandle);
};

USTRUCT(BlueprintType)
struct ABILITYFRAMEWORK_API FAFAttributesProperty
{
	GENERATED_BODY()
public:
	UPROPERTY(EditAnywhere)
		TSubclassOf<UGAAttributesBase> Attributes;
	/*
		If set, any values in Attributes will be overriden by table.
	*/
	UPROPERTY(EditAnywhere, BlueprintReadOnly)
		UDataTable* AttributeValues;
};
