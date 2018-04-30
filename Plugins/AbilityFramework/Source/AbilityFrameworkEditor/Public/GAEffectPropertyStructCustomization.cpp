// Copyright 1998-2014 Epic Games, Inc. All Rights Reserved.

#include "AbilityFrameworkEditor.h"
#include "IDetailsView.h"
#include "PropertyEditorModule.h"
#include "Editor/PropertyEditor/Public/PropertyEditing.h"
#include "Editor/PropertyEditor/Private/IDetailsViewPrivate.h"
#include "Editor/PropertyEditor/Private/SDetailsViewBase.h"
#include "Editor/PropertyEditor/Private/SDetailsView.h"
//D:\Unreal\UnrealEngine-Master\Engine\Source\Editor\PropertyEditor\Private\SDetailsView.h
#include "STextCombobox.h"
#include "STreeView.h"
#include "SButton.h"
#include "STextBlock.h"

#include "ClassViewerModule.h"
#include "ClassViewerFilter.h"

#include "EditorClassUtils.h"
#include "IAssetTools.h"
#include "AssetRegistryModule.h"
#include "Dialogs/DlgPickAssetPath.h"
#include "AssetToolsModule.h"
#include "KismetCompilerModule.h"
#include "Kismet2/KismetEditorUtilities.h"
#include "EffectEditor/GAEffectEditor.h"
#include "SMyBlueprint.h"
#include "SKismetInspector.h"

#include "SGAAttributeWidget.h"
#include "Effects/GAGameEffect.h"
#include "Effects/GAEffectBlueprint.h"
#include "EffectEditor/GAEffectBlueprintFactory.h"
#include "GAEffectPropertyStructCustomization.h"
#include "GAEffectDetails.h"

TSharedRef<IPropertyTypeCustomization> FGAEffectPropertyStructCustomization::MakeInstance()
{
	return MakeShareable(new FGAEffectPropertyStructCustomization);
}

FGAEffectPropertyStructCustomization::~FGAEffectPropertyStructCustomization()
{

}

void FGAEffectPropertyStructCustomization::CustomizeHeader(TSharedRef<IPropertyHandle> InStructPropertyHandle, FDetailWidgetRow& HeaderRow, IPropertyTypeCustomizationUtils& StructCustomizationUtils)
{
	const TArray<TWeakObjectPtr<UObject>>& objs = StructCustomizationUtils.GetPropertyUtilities()->GetSelectedObjects();
	UObject* Owner = nullptr;
	if (objs.Num() > 0)
	{
		Owner = objs[0].Get();
	}
	StructPropertyHandle = InStructPropertyHandle;
	TAttribute<FText> EffectName;// ();
	EffectName.Create(TAttribute<FText>::FGetter::CreateRaw(this, &FGAEffectPropertyStructCustomization::GetClassName));
	/*SNew(SVerticalBox)
		+ SVerticalBox::Slot()
		[
			SNew(STextBlock)
			.Text(EffectName)
		]*/
	EffectPropertyHandle = InStructPropertyHandle->GetChildHandle(TEXT("SpecClass")); //struct
	TSharedPtr<IPropertyHandle> ClassInStruct = EffectPropertyHandle->GetChildHandle(TEXT("SpecClass"));
	EffectClassWidget = MakeShareable(new FGAEffectClassStructWidget());
	EffectClassWidget->SetHandles(EffectPropertyHandle, ClassInStruct);
	
	HeaderRow
		.NameContent()
		[
			InStructPropertyHandle->CreatePropertyNameWidget()
		]
		.ValueContent()
		.HAlign(HAlign_Fill)
		[
			SNew(SHorizontalBox)
			+SHorizontalBox::Slot()
			.AutoWidth()
			[
				EffectClassWidget->CreateEffectClassWidget(Owner)
			]
		];
}
void FGAEffectPropertyStructCustomization::CustomizeChildren(TSharedRef<IPropertyHandle> InStructPropertyHandle, IDetailChildrenBuilder& StructBuilder, IPropertyTypeCustomizationUtils& StructCustomizationUtils)
{

}
FText FGAEffectPropertyStructCustomization::GetClassName() const
{
	if (StructPropertyHandle.IsValid())
	{
		return StructPropertyHandle->GetPropertyDisplayName();
	}
	return FText::FromString("No Effect Selected");
}