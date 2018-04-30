// Fill out your copyright notice in the Description page of Project Settings.

#include "SAFDAttributes.h"
#include "SlateOptMacros.h"
#include "SListView.h"
#include "SGridPanel.h"

BEGIN_SLATE_FUNCTION_BUILD_OPTIMIZATION
void SAFDAttributes::Construct(const FArguments& InArgs)
{
	AFInterface = InArgs._AbilityInterface;
	ChildSlot
		.HAlign(EHorizontalAlignment::HAlign_Fill)
		.VAlign(EVerticalAlignment::VAlign_Fill)
	[
		SNew(SOverlay)
		+SOverlay::Slot()
		.HAlign(EHorizontalAlignment::HAlign_Fill)
		.VAlign(EVerticalAlignment::VAlign_Fill)
		[
			SNew(SListView<TSharedPtr<FAttributeRow>>)
			.ListItemsSource(&Attributes)
			.OnGenerateRow(this, &SAFDAttributes::GenerateListRow)
		]
	];
	GatherAttributes();
}
void SAFDAttributes::GatherAttributes()
{
	if (!AFInterface)
		return;

	UGAAttributesBase* AttributesObject = AFInterface->GetAttributes();
	if (!AttributesObject)
		return;

	Attributes.Empty();
	for(TFieldIterator<UStructProperty> It(AttributesObject->GetClass(), EFieldIteratorFlags::IncludeSuper); It; ++It)
	{
		TSharedPtr<FAttributeRow> AttributeRow = MakeShareable(new FAttributeRow);
		UStructProperty* Prop = *It;
		FAFAttributeBase* Attr = Prop->ContainerPtrToValuePtr<FAFAttributeBase>(AttributesObject);
		if (Attr)
		{
			AttributeRow->Attribute = Attr;
			AttributeRow->Name = It->GetName();
			AttributeRow->Value = TAttribute<FText>::Create(TAttribute<FText>::FGetter::CreateSP(AttributeRow.Get(), &FAttributeRow::GetValue));
		}
		Attributes.Add(AttributeRow);
	}

}
TSharedRef<ITableRow> SAFDAttributes::GenerateListRow(TSharedPtr<FAttributeRow> NotifyName, const TSharedRef<STableViewBase>& OwnerTable)
{
	return
		SNew(STableRow< TSharedRef<FSoftObjectPath> >, OwnerTable)
		[
			SNew(SGridPanel)
			.FillColumn(0, 1)
			+SGridPanel::Slot(0,0)
			.HAlign(EHorizontalAlignment::HAlign_Fill)
			[
				SNew(STextBlock)
				.Text(FText::FromString(NotifyName->Name))
			]
			+ SGridPanel::Slot(1, 0)
			.HAlign(EHorizontalAlignment::HAlign_Fill)
			[
				SNew(STextBlock)
				.Text(NotifyName->Value)
			]
		];
}
END_SLATE_FUNCTION_BUILD_OPTIMIZATION
