#pragma once
#include "SlateBasics.h"
#include "SGraphPin.h"

class SGAAttributePin : public SGraphPin
{
public:
	SLATE_BEGIN_ARGS(SGAAttributePin) {}
	SLATE_END_ARGS()

public:
	void Construct(const FArguments& InArgs, UEdGraphPin* InGraphPinObj);

	virtual TSharedRef<SWidget>	GetDefaultValueWidget() override;
	void OnAttributeSelected(FString ItemSelected);
private:
	TArray<TSharedPtr<FString>> AttributesList;

	TSharedPtr<class SComboButton> AttributeComboButton;

	TSharedRef<SWidget> GetAttributesMenu();

	FText GetSelectedAttributeText() const;

	FString SelectedAttribute;

};