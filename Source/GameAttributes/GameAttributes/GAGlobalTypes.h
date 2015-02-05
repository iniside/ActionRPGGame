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

	Invalid
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

USTRUCT(BlueprintType)
struct GAMEATTRIBUTES_API FGAAttributeSpec
{
	GENERATED_USTRUCT_BODY()
public:
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "ModSpec")
		FGAAttribute Attribute;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "ModSpec")
		float ModValue;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "ModSpec")
		EGAAttributeOp Operation;
	//really ?
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Mod")
		FGameplayTagContainer Tags;

	FGAAttributeSpec()
	{
		ModValue = 0;
		Operation = EGAAttributeOp::Add;
	}
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
