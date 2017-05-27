// Copyright 1998-2014 Epic Games, Inc. All Rights Reserved.
#pragma once
#include "Attributes/GAAttributeGlobals.h"
#include "GAGlobalTypesEditor.h"

#include "IPropertyTypeCustomization.h"
#include "PropertyHandle.h"

class FGAEffectSpecStructCustomization : public IPropertyTypeCustomization
{
protected:
	TSharedPtr<IPropertyHandle> StructPropertyHandle;
	TSharedPtr<IPropertyHandle> EffectPropertyHandle;
	TSharedPtr<class SComboButton> ComboButton;

	TSharedPtr<SWindow> EffectEditorWindow;
public:
	TSubclassOf<class UGAGameEffectSpec> ParentClass;
	static TSharedRef<IPropertyTypeCustomization> MakeInstance();
	/**
	* Destructor
	*/
	virtual ~FGAEffectSpecStructCustomization();

	/** IPropertyTypeCustomization interface */
	virtual void CustomizeHeader(TSharedRef<IPropertyHandle> InStructPropertyHandle, FDetailWidgetRow& HeaderRow, IPropertyTypeCustomizationUtils& StructCustomizationUtils) override;
	virtual void CustomizeChildren(TSharedRef<IPropertyHandle> InStructPropertyHandle, IDetailChildrenBuilder& StructBuilder, IPropertyTypeCustomizationUtils& StructCustomizationUtils) override;

	FText GetClassName() const;
	FText GetDisplayValueAsString() const;
	TSharedRef<SWidget> GenerateClassPicker();
	void OnClassPicked(UClass* InClass);
	void SendToObjects(const FString& NewValue);
	//FReply OnDrop(const FGeometry& MyGeometry, const FDragDropEvent& DragDropEvent);
	TSharedRef<SWidget> MakeNewBlueprintButton();
	FReply MakeNewBlueprint();
	FReply OnEditButtonClicked();
	FReply OnCloseButtonClicked();
};