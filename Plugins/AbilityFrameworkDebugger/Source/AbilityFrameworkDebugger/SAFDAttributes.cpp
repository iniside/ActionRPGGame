// Fill out your copyright notice in the Description page of Project Settings.

#include "SAFDAttributes.h"
#include "SlateOptMacros.h"
#include "SListView.h"
#include "Attributes/GAAttributesBase.h"

BEGIN_SLATE_FUNCTION_BUILD_OPTIMIZATION
void SAFDAttributes::Construct(const FArguments& InArgs)
{
	AFInterface = InArgs._AbilityInterface;
	ChildSlot
	[
		SNew(SOverlay)
		+SOverlay::Slot()
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
	for(TFieldIterator<UProperty> It(AttributesObject->GetClass(), EFieldIteratorFlags::IncludeSuper); It; ++It)
	{
		TSharedPtr<FAttributeRow> AttributeRow = MakeShareable(new FAttributeRow);
		AttributeRow->Name = It->GetName();
		Attributes.Add(AttributeRow);
	}

}
TSharedRef<ITableRow> SAFDAttributes::GenerateListRow(TSharedPtr<FAttributeRow> NotifyName, const TSharedRef<STableViewBase>& OwnerTable)
{
	return
		SNew(STableRow< TSharedRef<FSoftObjectPath> >, OwnerTable)
		[
			SNew(SBox)
			.HAlign(HAlign_Left)
			[
				SNew(STextBlock)
				.Text(FText::FromString(NotifyName->Name))
			]
		];
}
END_SLATE_FUNCTION_BUILD_OPTIMIZATION
