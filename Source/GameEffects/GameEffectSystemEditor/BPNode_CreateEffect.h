#pragma once
#include "SlateBasics.h"
#include "EditorStyle.h"
#include "K2Node.h"
#include "K2Node_ConstructObjectFromClass.h"
#include "BPNode_CreateEffect.generated.h"


/*
	Update node to also apply effect.
*/

UCLASS()
class GAMEEFFECTSYSTEMEDITOR_API UBPNode_CreateEffect : public UK2Node_ConstructObjectFromClass
{
	GENERATED_UCLASS_BODY()

	// Begin UEdGraphNode interface.
	virtual void AllocateDefaultPins() override;
	virtual FLinearColor GetNodeTitleColor() const override;
	virtual void ExpandNode(class FKismetCompilerContext& CompilerContext, UEdGraph* SourceGraph) override;
	// End UEdGraphNode interface.

	// Begin UK2Node interface
	void GetMenuEntries(FGraphContextMenuBuilder& ContextMenuBuilder) const;
	virtual FText GetMenuCategory() const override;
	// End UK2Node interface.

	UEdGraphPin* GetObjectContexPin() const;
	UEdGraphPin* GetCauserPin() const;
	UEdGraphPin* GetTargetPin() const;
	UEdGraphPin* GetInstigatorPin() const;
protected:
	/** Gets the default node title when no class is selected */
	virtual FText GetBaseNodeTitle() const;
	/** Gets the node title when a class has been selected. */
	virtual FText GetNodeTitleFormat() const;
	/** Gets base class to use for the 'class' pin.  UObject by default. */
	virtual UClass* GetClassPinBaseClass() const;
	/**  */
	virtual bool IsSpawnVarPin(UEdGraphPin* Pin) override;
};
