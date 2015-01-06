#pragma once
#include "SlateBasics.h"
#include "EditorStyle.h"
#include "K2Node.h"
#include "K2Node_SpawnActorFromClassBase.h"
#include "BPNode_CreateEffectField.generated.h"

UCLASS()
class GAMEABILITIESEDITOR_API UBPNode_CreateEffectField : public UK2Node_SpawnActorFromClassBase
{
	GENERATED_UCLASS_BODY()

// Begin UEdGraphNode interface.
	virtual void AllocateDefaultPins() override;
	virtual FLinearColor GetNodeTitleColor() const override;
	virtual FText GetNodeTitle(ENodeTitleType::Type TitleType) const override;
	virtual void PinDefaultValueChanged(UEdGraphPin* Pin) override;
	virtual void PinConnectionListChanged(UEdGraphPin* Pin) override;
	virtual FText GetTooltipText() const override;
	virtual void ExpandNode(class FKismetCompilerContext& CompilerContext, UEdGraph* SourceGraph) override;
	virtual FName GetPaletteIcon(FLinearColor& OutColor) const override{ return TEXT("GraphEditor.SpawnActor_16x"); }
	// End UEdGraphNode interface.

	// Begin UK2Node interface
	virtual bool IsNodeSafeToIgnore() const override { return true; }
	virtual void GetNodeAttributes(TArray<TKeyValuePair<FString, FString>>& OutNodeAttributes) const override;
	virtual FText GetMenuCategory() const override;
	
	virtual void GetMenuEntries(FGraphContextMenuBuilder& ContextMenuBuilder) const override;
	// End UK2Node interface

	/** See if this is a spawn variable pin, or a 'default' pin */
	virtual bool IsSpawnVarPin(UEdGraphPin* Pin) override;

	/** Get the then output pin */
	virtual UEdGraphPin* GetThenPin() const override;
	/** Get the blueprint input pin */
	virtual UEdGraphPin* GetClassPin(const TArray<UEdGraphPin*>* InPinsToSearch = NULL) const override;

	virtual UEdGraphPin* GetResultPin() const override;
	/** Get the spawn transform input pin */
	UEdGraphPin* GetLocationPin() const;

	UEdGraphPin* GetInstigatorPin() const;
protected:
	virtual void OnClassPinChanged() override;
};
