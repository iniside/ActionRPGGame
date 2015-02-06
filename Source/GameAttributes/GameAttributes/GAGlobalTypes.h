#pragma once
#include "GameplayTagContainer.h"
#include "GAGlobalTypes.generated.h"

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
/**
 *	Struct representing single attribute. Needed for Pin customization.
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
Contains information on who we modify attribute
and from who modification originated.
*/
USTRUCT(BlueprintType)
struct GAMEATTRIBUTES_API FGAttributeContext
{
	GENERATED_USTRUCT_BODY()
public:
	UPROPERTY()
		TWeakObjectPtr<class UGAAttributeComponent> Target;
	UPROPERTY()
		TWeakObjectPtr<class UGAAttributeComponent> Instigator;
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
struct FGAAttributeMagnitudeSpec
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
		Data tabale from which we can just get, precalculated value, for this attribute. Hmm ?
		The real problems starts when we have more than one participating attribute in calculation.
		(Intelligence + Wisdom) * Value. Anyone ?
	*/
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Magnitude Calc")
		UDataTable* MagnitudeTable;
};
USTRUCT(BlueprintType)
struct FGAAttributeMagnitude
{
	GENERATED_USTRUCT_BODY()
public:
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Magnitude Calc")
		FGAAttributeMagnitudeSpec MagnitudeSpec;

	FGAttributeContext AttributeContext;
public:
	float CalculateMagnitude();

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

	//really ?
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Mod")
		FGameplayTagContainer Tags;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta = (FixedIncrement = 0.1), Category = "ModSpec")
		float ModValue;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta = (EditCondition = "bUseMagnitude"), Category = "ModSpec")
		FGAAttributeMagnitude AttributeMagnitude;

	bool IsValid() const 
	{
		if (bUseMagnitude)
			return Attribute.IsValid();
		else
			return true;
	}
	float GetCurrentMagnitude();
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
	UPROPERTY()
		FGAttributeContext AttributeContext;

	UPROPERTY()
		FVector HitLocation;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Mod")
		TArray<FGAAttributeSpec> AttributeModSpec;
	/**
	 *	Tags for this modifier.
	 */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Mod")
		FGameplayTagContainer Tags;


	void InitializeModData();
	void CalculcateCurrentMods();
	void ApplyMod();

	FGAAttributeModData()
	{
	};
};
/*
	Final evaluated data, which contains final mod value and attribute to which it's going to be
	appilied. As well as target context.
*/
USTRUCT()
struct GAMEATTRIBUTES_API FGAEvalData
{
	GENERATED_USTRUCT_BODY()
public:
	FGAEvalData()
	{
	};
	FGAEvalData(const FGAttributeContext& AttributeContextIn)
		: AttributeContext(AttributeContextIn)
	{
	};
	UPROPERTY()
		FGAAttribute Attribute;
	
	float ModValue;
	UPROPERTY()
		FGAttributeContext AttributeContext;
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
