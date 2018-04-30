// Copyright 1998-2014 Epic Games, Inc. All Rights Reserved.
#pragma once
#include "Attributes/GAAttributeGlobals.h"
#include "GAGlobalTypesEditor.h"

#include "IPropertyTypeCustomization.h"
#include "PropertyHandle.h"

class FGAEffectClassStructWidget : public TSharedFromThis<FGAEffectClassStructWidget>
{
protected:
	/* Handle to FGAEffectClass*/
	TSharedPtr<IPropertyHandle> StructPropertyHandle;

	/* Handle to TSubclassOf<UGAGameEffectSpec> SpecClass */
	TSharedPtr<IPropertyHandle> EffectPropertyHandle;
	
	TSharedPtr<class SComboButton> ComboButton;

	TSharedPtr<SWindow> EffectEditorWindow;
	UBlueprint* EditedBlueprint;
	UObject* OuterObject;
public:
	FGAEffectClassStructWidget() {};
	FGAEffectClassStructWidget(TSharedPtr<IPropertyHandle> InStructPropertyHandle,
		TSharedPtr<IPropertyHandle> InEffectPropertyHandle)
		: StructPropertyHandle(InStructPropertyHandle),
		EffectPropertyHandle(InEffectPropertyHandle)
	{};

	TSubclassOf<class UGAGameEffectSpec> ParentClass;
	TSharedRef<SWidget> CreateEffectClassWidget(UObject* OwnerObject);
	void SetHandles(TSharedPtr<IPropertyHandle> InStructPropertyHandle,
		TSharedPtr<IPropertyHandle> InEffectPropertyHandle)
	{
		StructPropertyHandle = InStructPropertyHandle;
		EffectPropertyHandle = InEffectPropertyHandle;
	}
	
	/**
	* Destructor
	*/
	virtual ~FGAEffectClassStructWidget();

	FText GetClassName() const;
	FText GetDisplayValueAsString() const;
	UClass* GetClassFromString(const FString& ClassName);
	TSharedRef<SWidget> GenerateClassPicker();
	void OnClassPicked(UClass* InClass);
	void SendToObjects(const FString& NewValue);
	//FReply OnDrop(const FGeometry& MyGeometry, const FDragDropEvent& DragDropEvent);
	TSharedRef<SWidget> MakeNewBlueprintButton();
	FReply MakeNewBlueprint();
	FReply OnEditButtonClicked();
	FReply OnSaveButtonClicked();
	FReply OnSaveCloseButtonClicked();
	FReply OnCloseButtonClicked();
};