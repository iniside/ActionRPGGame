#pragma once
#include "GAGlobalTypes.h"
#include "GAAttributeMod.generated.h"

UCLASS(BlueprintType, Blueprintable, DefaultToInstanced, EditInlineNew)
class GAMEATTRIBUTES_API UGAAttributeMod : public UObject
{
	GENERATED_UCLASS_BODY()
public:
	UFUNCTION(BlueprintImplementableEvent)
		void OnAttributeModifyOut(const FGAAttributeModifier& AttributeParam, FGAAttributeModifier& AttributeOut);
	
	UFUNCTION(BlueprintImplementableEvent)
		void OnAttributeModifyIn(const FGAAttributeModifier& AttributeParam, FGAAttributeModifier& AttributeIn);

	void BindDelegates(FGAOnAttributeOutgoing& Outgoing, FGAOnAttributeIncoming& Incoming);
};
