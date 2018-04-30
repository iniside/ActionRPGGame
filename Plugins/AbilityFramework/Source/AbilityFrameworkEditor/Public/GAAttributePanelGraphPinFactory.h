#pragma once
#include "SlateBasics.h"
#include "Attributes/GAAttributeGlobals.h"
#include "EdGraph/EdGraphPin.h"
#include "EdGraph/EdGraphSchema.h"
#include "EdGraphSchema_K2.h"
#include "GAAttributePanelGraphPinFactory.h"
#include "GAAttributePin.h"
#include "EdGraphUtilities.h"

class FGAAttributePanelGraphPinFactory : public FGraphPanelPinFactory
{
	virtual TSharedPtr<class SGraphPin> CreatePin(class UEdGraphPin* InPin) const override
	{
		const UEdGraphSchema_K2* K2Schema = GetDefault<UEdGraphSchema_K2>();
		if (InPin->PinType.PinCategory == K2Schema->PC_Struct
			&& InPin->PinType.PinSubCategoryObject == FGAAttribute::StaticStruct())
		{
			return SNew(SGAAttributePin, InPin);
		}
		return nullptr;
	}
};