#pragma once
#include "GameplayTagContainer.h"
#include "GAGlobalTypes.generated.h"


/*
	That file is veryyyy WIP. I'm refactoring entire code!
*/
/**/
UENUM()
enum class EGAAttributeOp : uint8
{
	Add,
	Subtract,
	Multiply,
	Divide,
	Set,
	Precentage,

	Invalid
};

UENUM()
enum class EGAAttributeSource : uint8
{
	Instigator,
	Target
};

UENUM()
enum class EGAMagnitudeOperation : uint8
{
	Add,
	Subtract,
	Multiply,
	Divide,

	Invalid
};

USTRUCT(BlueprintType)
struct FGAttributeMagnitudeData: public FTableRowBase
{
	GENERATED_USTRUCT_BODY()

public:
	/** Preculculated magnitude associated with this attribite level. */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Magnitude)
		float Magnitude;

	FGAttributeMagnitudeData()
	{}
};
USTRUCT(BlueprintType)
struct FGAEffectHandle
{
	GENERATED_USTRUCT_BODY()
protected:
	UPROPERTY()
		int32 Handle;

public:
	FGAEffectHandle()
		: Handle(INDEX_NONE)
	{}
	FGAEffectHandle(int32 HandleIn)
		: Handle(HandleIn)
	{}
	bool IsValid()
	{
		return Handle != INDEX_NONE;
	}

	static FGAEffectHandle GenerateHandle();

	bool operator==(const FGAEffectHandle& Other) const
	{
		return Handle == Other.Handle;
	}
	bool operator!=(const FGAEffectHandle& Other) const
	{
		return Handle != Other.Handle;
	}
	friend uint32 GetTypeHash(const FGAEffectHandle& InHandle)
	{
		return InHandle.Handle;
	}

};

/*
	Struct representing final modifier applied to attribute.
*/
struct FGAModifier
{
	EGAAttributeOp AttributeMod;
	float Value;
};
/**
 *	Struct representing single attribute. Needed for Pin customization.
 *	What we really need is to use this struct for making "complex" attributes.
 *	Which means attributes, which needs to track their own state.
 *	Bonuses applied to them, types of bonues, and who applied those bonuses
 *	so we can properly remove them, get them, track them, and controll order
 *	in which theyare added.
 *	we will two have attributes types. Staless (transient), auxiallry attttributes
 *	and staefull attributes, which are going to track their state.
 */
USTRUCT(BlueprintType)
struct GAMEATTRIBUTES_API FGAAttribute
{
	GENERATED_USTRUCT_BODY()
public:
	UPROPERTY(EditAnywhere, BlueprintReadOnly)
		FName AttributeName;

	inline bool operator== (const FGAAttribute& OtherAttribute) const
	{
		return (OtherAttribute.AttributeName == AttributeName);
	}

	inline bool operator!= (const FGAAttribute& OtherAttribute) const
	{
		return (OtherAttribute.AttributeName != AttributeName);
	}

	inline bool IsValid() const
	{
		return !AttributeName.IsNone();
	}

	FGAAttribute()
	{
		AttributeName = NAME_None;
	};
	FGAAttribute(const FName& AttributeNameIn)
	{
		AttributeName = AttributeNameIn;
	};
};

USTRUCT(BlueprintType)
struct GAMEATTRIBUTES_API FGAAttributeBase
{
	GENERATED_USTRUCT_BODY()
public:
	UPROPERTY()
		FName AttributeName;
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Value")
		float BaseValue;
	/*
		This is maxmum value of this attribute.
	*/
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Value")
		float ClampValue;
protected:
	float BonusValue;
	//float OldCurrentValue;
	float CurrentValue;
	float ChangedValue;

	//map of modifiers.
	//It could be TArray, but map seems easier to use in this case
	//we need to keep track of added/removed effects, and see 
	//if this effect affected this attribute.
	TMap<FGAEffectHandle, FGAModifier> Modifiers;

public:
	
	inline float GetFinalValue(){ return BaseValue + BonusValue; };
	inline float GetCurrentValue(){ return CurrentValue; };
	void UpdateAttribute();

	void Add(float ValueIn);
	void Subtract(float ValueIn);

	//inline float GetCurrentValue()
	//{
	//	return FMath::Clamp<float>(BaseValue + BonusValue + AccumulatedBonus, 0, GetFinalValue());;
	//}
	
	void AddBonus(const FGAModifier& ModifiersIn, const FGAEffectHandle& Handle);
	void RemoveBonus(const FGAEffectHandle& Handle);

	void InitializeAttribute();

	void CalculateBonus();

	inline bool operator== (const FGAAttributeBase& OtherAttribute) const
	{
		return (OtherAttribute.AttributeName == AttributeName);
	}

	inline bool operator!= (const FGAAttributeBase& OtherAttribute) const
	{
		return (OtherAttribute.AttributeName != AttributeName);
	}

	inline bool IsValid() const
	{
		return !AttributeName.IsNone();
	}

	FGAAttributeBase()
		: AttributeName(NAME_None),
		BaseValue(0),
		BonusValue(0),
		ChangedValue(0)
	{
	};
	FGAAttributeBase(const FName& AttributeNameIn)
		: AttributeName(AttributeNameIn),
		BaseValue(0),
		BonusValue(0),
		ChangedValue(0)
	{
	};
	FGAAttributeBase(const FName& AttributeNameIn, float BaseValueIn)
		: AttributeName(AttributeNameIn),
		BaseValue(BaseValueIn),
		BonusValue(0),
		ChangedValue(0)
	{
	};
	friend uint32 GetTypeHash(const FGAAttributeBase& AttributeIn)
	{
		return AttributeIn.AttributeName.GetComparisonIndex();
	}
};

USTRUCT(BlueprintType)
struct FGAEffectContext
{
	GENERATED_USTRUCT_BODY()
public:
	/**
	 *	Target which we apply Effect;
	 */
	UPROPERTY(BlueprintReadOnly, Category = "Spec")
		FHitResult Target;
	/**
	 *	Direct Reference to TargetActor (I will possibly remove FHitResult Target!
	 */
	UPROPERTY(BlueprintReadOnly, Category = "Spec")
		TWeakObjectPtr<AActor> TargetActor;
	/**
	 *	Object which caused this effect. Might be ability, weapon, projectile etc.
	 */
	UPROPERTY(BlueprintReadOnly, Category = "Spec")
		TWeakObjectPtr<UObject> Causer;
	/**
	 *	Pawn, which originally instigated effect (an owned AttributeComponent).
	 */
	UPROPERTY(BlueprintReadOnly, Category = "Spec")
		TWeakObjectPtr<APawn> Instigator;
	/**
	 *	Attribute component of Target.
	 */
	UPROPERTY(BlueprintReadOnly, Category = "Spec")
		TWeakObjectPtr<class UGAAttributeComponent> TargetComp;
	/**
	 *	Attribute component of Intigator
	 */
	UPROPERTY(BlueprintReadOnly, Category = "Spec")
		TWeakObjectPtr<class UGAAttributeComponent> InstigatorComp;
};

USTRUCT(BlueprintType)
struct GAMEATTRIBUTES_API FGAUpdatedAttribute
{
	GENERATED_USTRUCT_BODY()
public:
	UPROPERTY(BlueprintReadOnly)
		FGAAttribute Attribute;

	UPROPERTY(BlueprintReadOnly)
		float NewValue;
};

/*
	Must be blueprint type, for supporting creating PostAttribute_
	functions;
*/
USTRUCT(BlueprintType)
struct GAMEATTRIBUTES_API FGAAttributeDataCallback
{
	GENERATED_USTRUCT_BODY()
public:

	//attribute we changed
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Callback")
		FGAAttribute Attribute;
	//final mod we appilied to this attribute..
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Callback")
	float AttributeValue;

	/*
	attribute, which Attribute modified.
	Simply speaking if we have Modified attribute Damage by 10, target had 100 Health.
	If damage Modify Health, then this attribute will have value
	Health,
	and ModifiedAttributeValue will be 90
	*/
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Callback")
		FGAAttribute ModifiedAttribute;

	//final value of attribute, which AttributeModified
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Callback")
	float ModifiedAttributeValue;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Callback")
		FGAEffectContext AttributeContext;
	FGAAttributeDataCallback()
	{
	};
	FGAAttributeDataCallback(const FGAEffectContext& AttributeContextIn)
		: AttributeContext(AttributeContextIn)
	{
	};
};

/*
	Ok. What I really want to achieve here is:
	1. You get Attribute and Base value. You can make any arbitrary operation on it.
	2. You make next set, and do the same operation and the you make operation between two sets of resulting magnitueds.
	3. This way I would not have to deal with data tables.

	Either way I still need to figure out best workflow for it.

	Calculating magnitude inside Object/Actor is not that bad idea, as it cuts on huge amount of content otherwise needed
	(separate data curves/tables for each ability, and each ability can make use of multiple of them!).
	Once we have equation figured out in something like ability it's only matter of chaging base values, or affecting attributes in defaults.

	The advantage of using data table would increased performance, since we have everything precalculated already. It
	s simple matter of pulling right data (but we will need more memory).
	The downside is amount of assets to manage(, and possibility of making big mistakes like plugging wrong data table into wrong ability.
*/
/*
	I'm still playing around what's the best way of evaluating data here.
	And to store it.
*/

USTRUCT(BlueprintType)
struct FGAAttributeMagnitude
{
	GENERATED_USTRUCT_BODY()
public:
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Magnitude Calc")
		EGAAttributeSource AttributeSource;
	//so let say spec can contain It's own ModValue Calculations..
	//first Attribute against which we will try to calculate:
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Magnitude Calc")
		FGAAttribute AttributeMod;
	/*
		Curve Handle from which we will pull value for base magnitude. 
		Column is based of attribute value!
	*/
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Magnitude Calc")
		FCurveTableRowHandle MagnitudeCurve;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Backing Attribute")
		EGAAttributeSource BackingAttributeSource;


	/*
		Magnitude = Magnitude OPERATION BackingAttributeValue. 
		OR
		Magnitude = Magnitude OPERATION BackingAttrbuteMagnitude (from external source, like table)
	*/
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Backing Attribute")
		FGAAttribute BackingAttribute;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Backing Attribute")
		EGAMagnitudeOperation BackAttributeOperation;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Magnitude Calc")
		FCurveTableRowHandle BackingCurve;

	//condtional backing ? eee ?

public:
	FGAAttributeMagnitude()
	{
	}
};

USTRUCT(BlueprintType)
struct GAMEATTRIBUTES_API FGAAttributeSpec
{
	GENERATED_USTRUCT_BODY()
public:
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "ModSpec")
		FGAAttribute Attribute;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "ModSpec")
		EGAAttributeOp Operation;
	/*
		If false, you have to type ModValue Manually.
	*/
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "ModSpec")
		bool bUseMagnitude;

	//One attribute change - one tag.
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Mod")
		FGameplayTag AttributeTag;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta = (FixedIncrement = 0.1), Category = "ModSpec")
		float ModValue;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta = (EditCondition = "bUseMagnitude"), Category = "ModSpec")
		FGAAttributeMagnitude AttributeMagnitude;


	FGAModifier FinalMod;
	bool operator==(const FGAAttributeSpec& OtherIn) const
	{
		return OtherIn.Attribute == Attribute && OtherIn.ModValue == ModValue;
	}

	bool IsValid() const 
	{
		if (bUseMagnitude)
			return Attribute.IsValid();
		else
			return true;
	}
	float GetCurrentMagnitude(const FGAEffectContext& Context);
	float CalcuclateMagnitude(const FGAEffectContext& Context);
protected:
	float CalculatedMagnitude;

public:
	inline float GetCalculatedMagnitude(){ return CalculatedMagnitude; };
	inline void SetCalculatedMagnitude(float ValueIn){ CalculatedMagnitude = ValueIn; };
	FGAAttributeSpec()
	{
		ModValue = 0;
		Operation = EGAAttributeOp::Add;
		bUseMagnitude = false;
		CalculatedMagnitude = 0;
		FinalMod.AttributeMod = EGAAttributeOp::Add;
		FinalMod.Value = 50;
	}
};



/**
 *	Who we modify.
 *	Who instigated this mod.
 *	What attributes we modify.
 */
USTRUCT(BlueprintType, meta = (DisplayName = "Attribute Modifier"))
struct GAMEATTRIBUTES_API FGAAttributeModData
{
	GENERATED_USTRUCT_BODY()
public:
	//UPROPERTY()
	//	FGAttributeContext AttributeContext;

	UPROPERTY()
		FVector HitLocation;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Mod")
		TArray<FGAAttributeSpec> AttributeModSpec;

	//will cache off attribute specs here for quick look ups,
	//so we can modify them faster.
	//TMap<FName, FGAAttributeSpec> AttributeSpecMap;

	void InitializeModData();
	void CalculcateCurrentMods();
	FGAAttributeDataCallback ApplyMod();

	bool IsValid()
	{
		for (const FGAAttributeSpec& spec : AttributeModSpec)
		{
			if (!spec.IsValid())
				return false;
		}
		return true;
	}

	FGAAttributeModData()
	{
	};
};
/*
	Final evaluated data, which contains final mod value and attribute to which it's going to be
	appilied. As well as target context.
*/
USTRUCT(BlueprintType)
struct GAMEATTRIBUTES_API FGAEvalData
{
	GENERATED_USTRUCT_BODY()
public:


	//attribute we changed
	UPROPERTY(BlueprintReadOnly)
		FGAAttribute Attribute;
	//final mod we appilied to this attribute..
	UPROPERTY(BlueprintReadOnly)
	float ModValue;

	UPROPERTY(BlueprintReadOnly)
		FGAEffectContext AttributeContext;
	FGAEvalData()
	{
	};
	FGAEvalData(const FGAEffectContext& AttributeContextIn)
		: AttributeContext(AttributeContextIn)
	{
	};
};

USTRUCT(BlueprintType)
struct GAMEATTRIBUTES_API FGAAttributeModSelf
{
	GENERATED_USTRUCT_BODY()
public:
	UPROPERTY(BlueprintReadWrite, Category = "Mod")
		TWeakObjectPtr<AActor> Target;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Mod")
		FGAAttribute Attribute;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Mod")
		float Value;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Mod")
		EGAAttributeOp Operation;

	/**
	*	Tags for this modifier.
	*/
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Mod")
		FGameplayTagContainer Tags;

	FGAAttributeModSelf()
	{
		Value = 0;
		Operation = EGAAttributeOp::Add;
	};
};
/*
	It's going to be helper replication struct
	mainly used to communicate changes to UI, so you can decide to what do with them.
*/
USTRUCT(BlueprintType)
struct GAMEATTRIBUTES_API FGAModifiedAttribute
{
	GENERATED_USTRUCT_BODY()
public:
	/**
	 *	Always increment it, to make sure it will replicate.
	 */
	UPROPERTY()
		int8 ReplicationCounter;
	
	/**
	 *	Attribute we have modified.
	 */
	UPROPERTY()
		FGAAttribute Attribute;
	
	/**
	 *	Final value by which we modified attribute.
	 */
	UPROPERTY()
		float ModifiedByValue;
	
	/**
	 *	Final tags appiled by this change.
	 */
	UPROPERTY()
		FGameplayTagContainer Tags;

	UPROPERTY()
		FVector TargetLocation; //change to vector, we need only position.
	UPROPERTY()
		FVector InstigatorLocation;

	UPROPERTY()
		TWeakObjectPtr<class UGAAttributeComponent> Causer;
};

DECLARE_MULTICAST_DELEGATE_OneParam(FGAOnAttributeUpdated, const FGAUpdatedAttribute&);



UENUM()
enum class EGAEffectType : uint8
{
	Instant,
	Periodic,
	Duration,
	Infinite
};

UENUM()
enum class EGAEffectStacking : uint8
{
	Replace, //will end previous effect, and replace with new one.
	Restart, //will restart existing effect
	Duration, //will add duration to existing effect
	Intensity, //will add magnitude to existing effect.
	Add, //will simply add new effect

	Invalid
};

UENUM()
enum class EGAEffectModPolicy : uint8
{
	Permament, //mod appilies permament changes to attribute, we will never restore value after we expire.
	//be careful with it!
	Restore //if/when we expire we will restore attribute by mod we modified it while
	//we have been active.
};

USTRUCT(BlueprintType)
struct FGAEffectPolicy
{
	GENERATED_USTRUCT_BODY()
public:
	UPROPERTY(EditAnywhere, Category = "Policy")
		EGAEffectModPolicy ModPolicy;
	UPROPERTY(EditAnywhere, Category = "Policy")
		EGAEffectType EffectType;
	UPROPERTY(EditAnywhere, Category = "Policy")
		EGAEffectStacking EffectStacking;
	UPROPERTY(EditAnywhere, Category = "Policy")
		uint32 bInstanceEffect : 1;
};

UENUM()
enum class EGAEffectModType : uint8
{
	Add, //flat add mod
	Multiply, //Multiply by mod
	Subtract, //subtract mod
	Divide, //Divide mod
	PrecentageReduce, //calculate prcentage from base value and add it
	PrecentageIncrease,
};


/*
I needed dedicated effect struct, which will support at least some
basic polymorphism.

I need some simple build in effects,
and ability to easily extends.

These structs are not going to be expsed to blueprint directly,
instead we will expose "configuration" structs, and we will construct proper
struct using correct static function.

1. No hard references *.
*/
USTRUCT(BlueprintType)
struct GAMEATTRIBUTES_API FGAEffectBase
{
	GENERATED_USTRUCT_BODY()
public:
	/*
		Provide effect class, if you need more complicated conditional execution path.
	*/
	UPROPERTY(EditAnywhere, Category = "Spec")
		TSubclassOf<class UGAEffect> EffectClass;
	/*
		Create new instance of EffectClass ?
		If false, only default class object will be provieded.
		If true, there will be new effect instance created, which have access to event graph executions.

		Either way, instanced effect is never replicated back to clients (might change it, but dunno).
	*/
	UPROPERTY(EditAnywhere, Category = "Spec")
		bool bInstanceEffect;

	UPROPERTY()
		TWeakObjectPtr<class UGAEffect> EffectInstance;

	/*
		Contains Information About Target/Instigator
	*/
	UPROPERTY(BlueprintReadOnly, Category = "Spec")
		FGAEffectContext EffectContext;

	/*
		Who appilied me ? Might be for example tag of ability Ability.Fireball, Ability.WallOfFire.
	*/
	UPROPERTY(EditAnywhere, Category = "Tags")
		FGameplayTag CauserTag;
	/*
		My tag. For example Hex, Enchatment, Condition.Weakness, Condition.Bleed, Condition.Burning etc.
	*/
	UPROPERTY(EditAnywhere, Category = "Tags")
		FGameplayTag MyTag;
	/*
		I will apply these immunit tags, to target.
	*/
	UPROPERTY(EditAnywhere, Category = "Tags")
		FGameplayTagContainer ImmunityTags;
	/*
		These tags must be present on actor, if I'm to take any effect.

		If empty I will be applied regardless of tags owned by target.
	*/
	UPROPERTY(EditAnywhere, Category = "Tags")
		FGameplayTagContainer RequiredTags;
	/*
		I will add these tags, to target, when I'm applied.
	*/
	UPROPERTY(EditAnywhere, Category = "Tags")
		FGameplayTagContainer ApplyTags;
	//prevention tags ? if target have ny of these teg this effect would not apply
	//it would essentially work like immunity tag, just from effect side, not from target.
	FGAEffectHandle Handle;


	inline TWeakObjectPtr<UGAAttributeComponent> GetTargetComp()
	{
		return EffectContext.TargetComp;
	}
	inline TWeakObjectPtr<UGAAttributeComponent> GetInstigatorComp()
	{
		return EffectContext.InstigatorComp;
	}
	inline TWeakObjectPtr<AActor> GetTarget()
	{
		return EffectContext.Target.Actor;
	}
	inline TWeakObjectPtr<APawn> GetInstigator()
	{
		return EffectContext.Instigator;
	}
	inline FVector GetTargetLocation()
	{
		return EffectContext.Target.Location;
	}
	FGAEffectBase()
	{

	}

	static const int32 EffectID = 0;
	virtual int32 GetEffectID() const { return FGAEffectBase::EffectID; };
	virtual bool IsOfType(int32 IDIn) const { return FGAEffectBase::EffectID == IDIn; }
};

DECLARE_MULTICAST_DELEGATE_TwoParams(FGAOnPostModifyAttribute, const FGAEvalData&, FGAAttributeDataCallback&);
DECLARE_MULTICAST_DELEGATE_OneParam(FGAOnPostEffectApplied, FGAEffectBase&);
DECLARE_MULTICAST_DELEGATE_OneParam(FGAOnPostEffectRemoved, FGAEffectBase&);
DECLARE_MULTICAST_DELEGATE_TwoParams(FGAOnCalculateOutgoingAttributeMods, const FGAAttributeSpec&, FGAAttributeSpec&);
DECLARE_MULTICAST_DELEGATE_TwoParams(FGAOnCalculateIncomingAttributeMods, const FGAAttributeSpec&, FGAAttributeSpec&);

USTRUCT(BlueprintType)
struct GAMEATTRIBUTES_API FGAEffectInstant : public FGAEffectBase
{
	GENERATED_USTRUCT_BODY()
public:
	/*
		Tag, associated with attribute.
		For example, Damage.Fire, Damage.Shadow, Damage.Ice
		Health.ReducePrecentage, Health.Exaustion.
	*/
	UPROPERTY(EditAnywhere, Category = "Tags")
		FGameplayTag AttributeTag;

	UPROPERTY(EditAnywhere, Category = "Spec")
		FGAAttributeModData AttributeData;

	static const int32 EffectID = 1;

	void CalculateMagnitude();

	//I actually need to route it trough something like
	//effect container.
	//to give existing effect chance to modify this effect.
	//same thing will apply for all other effects.
	void ApplyInstantEffect();

	FGAEffectInstant()
	{

	}


	virtual int32 GetEffectID() const override { return FGAEffectInstant::EffectID; };
	virtual bool IsOfType(int32 IDIn) const override { return FGAEffectInstant::EffectID == IDIn; }
};

USTRUCT(BlueprintType)
struct FGAEffectAttributeSpec
{
	GENERATED_USTRUCT_BODY()
public:
	UPROPERTY(EditAnywhere, Category = "Spec")
		FGAAttributeModData OnAppliedSpec;
	UPROPERTY(EditAnywhere, Category = "Spec")
		FGAAttributeModData OnContiniousSpec;
	UPROPERTY(EditAnywhere, Category = "Spec")
		FGAAttributeModData OnRemovedSpec;
	UPROPERTY(EditAnywhere, Category = "Spec")
		FGAAttributeModData OnExpiredSpec;
	bool IsValid()
	{
		if (!OnAppliedSpec.IsValid())
			return false;
		if (!OnContiniousSpec.IsValid())
			return false;
		if (!OnRemovedSpec.IsValid())
			return false;
		if (!OnExpiredSpec.IsValid())
			return false;
		return true;
	}
};

USTRUCT(BlueprintType)
struct FGAEffectDurationSpec
{
	GENERATED_USTRUCT_BODY()
public:
	UPROPERTY(EditAnywhere, Category = "Spec")
		FGAAttributeSpec DurationMagnitude;

	UPROPERTY(EditAnywhere, Category = "Spec")
		FGAAttributeSpec PeriodMagnitude;


	bool IsValid() const
	{
		return DurationMagnitude.IsValid() && PeriodMagnitude.IsValid();
	}
};

USTRUCT(BlueprintType)
struct FGAEffectModiferSpec
{
	GENERATED_USTRUCT_BODY()
public:
	/*
		Incoming attribute spec, must match any of these tags,
		if I'm to modify it.
	*/
	UPROPERTY(EditAnywhere, Category = "Mod")
		FGameplayTagContainer RequiredTags;

	UPROPERTY(EditAnywhere, Category = "Mod")
		EGAEffectModType ModType;
	/*
	falt out modifier value.

	Later we probabaly want to pull it from data table
	and/or add more options.
	*/
	UPROPERTY(EditAnywhere, Category = "Mod")
		float ModValue;


	void ModifySpec(FGAAttributeSpec& SpecIn);
};

USTRUCT(BlueprintType)
struct GAMEATTRIBUTES_API FGAEffectDuration : public FGAEffectBase
{
	GENERATED_USTRUCT_BODY()
public:
	UPROPERTY(EditAnywhere, Category = "Duration Spec")
		FGAEffectDurationSpec DurationSpec;

	UPROPERTY(EditAnywhere, Category = "Attribute Spec")
		FGAEffectAttributeSpec AttributeSpec;

	UPROPERTY(EditAnywhere, Category = "Attribute Spec")
		FGAEffectModiferSpec ModifierSpec;


	FTimerHandle DurationHandle;
	
	void ApplyEffect();
	void EffecExpired();
	void RegisterDelegates(FGAOnPostEffectApplied& PostEffectApplied, FGAOnPostEffectRemoved& PostEffectRemoved);

	void OnPostEffectApplied(FGAEffectBase& EffectIn);
	void OnPostEffectRemoved(FGAEffectBase& EffectIn);

	//effect has been succeefully applied to target;
	void OnApllied();
	//effect has continous effect(?) on target, like dot damage.
	void OnContinious(); //calculate magnitude on each "tick" or just once ?
	//effect has been prematurly removed by other offect or other means.
	void OnRemoved();
	//effect naturally expired (reached max duration).
	void OnExpired();

	void ModifyAttributeSpec(FGAAttributeSpec& SpecIn);

	static const int32 EffectID = 2;

	virtual int32 GetEffectID() const override { return FGAEffectDuration::EffectID; };
	virtual bool IsOfType(int32 IDIn) const override { return FGAEffectDuration::EffectID == IDIn; }
};
USTRUCT()
struct FGACountedTagContainer
{
	GENERATED_USTRUCT_BODY()
protected:
	/*
		Here we will store counted GamaplayTags. 
		If tag is already in map, we just incremement count.
	*/
	TMap<FGameplayTag, int32> CountedTags;

	/*
		Here we store all currently posesd tags.
		It is equivalent of CountedTags, except this does not track count of tags, but we need it
		to have something against which we can perform queries.
	*/
	FGameplayTagContainer AllTags;
public:
	void AddTag(const FGameplayTag& TagIn);
	void AddTagContainer(const FGameplayTagContainer& TagsIn);
	void RemoveTag(const FGameplayTag& TagIn);

	bool HasTag(const FGameplayTag& TagIn);
	bool HasAllTags(const FGameplayTagContainer& TagsIn, bool bCountEmptyAsMatch);
	bool HasAnyTags(const FGameplayTagContainer& TagsIn, bool bCountEmptyAsMatch);
};

USTRUCT()
struct FGAEffectContainer
{
	GENERATED_USTRUCT_BODY()
public:
};

USTRUCT()
struct FGAActiveEffectContainer
{
	GENERATED_USTRUCT_BODY()
public:
	void ApplyEffect(const FGAEffectContext& Ctx, FGAAttributeModData& SpecModifiers);

	TMap<FGAEffectHandle, TSharedPtr<FGAEffectDuration>> Effects;
};


