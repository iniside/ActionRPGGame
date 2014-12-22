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
	UPROPERTY(BlueprintReadOnly)
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

	FGAAttributeModifier()
	{
		Value = 0;
		Operation = EGAAttributeOp::Add;
	};
};