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

	FGAAttribute()
	{
		AttributeName = NAME_None;
	};
};

/**
 *	Struct defining what attribute to modify, by what value, and how.
 */
USTRUCT(BlueprintType)
struct GAMEATTRIBUTES_API FGAAttributeModifier
{
	GENERATED_USTRUCT_BODY()
public:
	UPROPERTY(BlueprintReadWrite, Category = "Mod")
		FHitResult HitTarget;

	UPROPERTY(BlueprintReadWrite, Category = "Mod")
		TWeakObjectPtr<AActor> Instigator;

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

	FGAAttributeModifier()
	{
		Value = 0;
		Operation = EGAAttributeOp::Add;
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


DECLARE_MULTICAST_DELEGATE_TwoParams(FGAOnAttributeOutgoing, const FGAAttributeModifier&, FGAAttributeModifier&);
DECLARE_MULTICAST_DELEGATE_TwoParams(FGAOnAttributeIncoming, const FGAAttributeModifier&, FGAAttributeModifier&);