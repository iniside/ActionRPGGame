#pragma once
#include "SlateBasics.h"
#include "EditorStyle.h"
#include "K2Node.h"
#include "K2Node_SpawnActorFromClass.h"
#include "BPNode_SpawnEffectField.generated.h"

UCLASS()
class GAMESYSTEMEDITOR_API UBPNode_SpawnEffectField : public UK2Node_SpawnActorFromClass
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

	UClass* GetClassToSpawn(const TArray<UEdGraphPin*>* InPinsToSearch = NULL) const;
	
	/** See if this is a spawn variable pin, or a 'default' pin */
	bool IsSpawnVarPin(UEdGraphPin* Pin);

	/** Get the then output pin */
	UEdGraphPin* GetThenPin() const;
	/** Get the blueprint input pin */
	UEdGraphPin* GetClassPin(const TArray<UEdGraphPin*>* InPinsToSearch = NULL) const;;

	UEdGraphPin* GetResultPin() const;
	/** Get the spawn transform input pin */
	UEdGraphPin* GetLocationPin() const;

	UEdGraphPin* GetInstigatorPin() const;
protected:
	void OnClassPinChanged();
};
