// Copyright 1998-2014 Epic Games, Inc. All Rights Reserved.

#include "TimeOfDayEditor.h"

#include "TODAssetDetails.h"

TSharedRef<IDetailCustomization> FTODAssetDetails::MakeInstance(FOnGetCurrentProperty OnGetCurrentPropertyIn)
{
	return MakeShareable(new FTODAssetDetails(OnGetCurrentPropertyIn));
}

void FTODAssetDetails::CustomizeDetails(IDetailLayoutBuilder& DetailLayout)
{
	const IDetailsView& DetailView = DetailLayout.GetDetailsView();

	TWeakObjectPtr<UObject> InspectedObject;

	for (TWeakObjectPtr<UObject> inspObj : DetailView.GetSelectedObjects())
	{
		InspectedObject = inspObj;
		break;
	}

	UTODAsset* TODAsset = Cast<UTODAsset>(InspectedObject.Get());

	if (TODAsset)
	{
		for (TFieldIterator<UProperty> PropIt(TODAsset->GetClass()); PropIt; ++PropIt)
		{
			UProperty* prop = *PropIt;
			DetailLayout.HideProperty(prop->GetFName());
		}
	}

	FName CurrentPropertyName = TEXT("SunIntensityCurve");// NAME_None;
	//if (OnGetCurrentProperty.IsBound())
	//{
	//	CurrentPropertyName = OnGetCurrentProperty.Execute();
	//}
	if (CurrentPropertyName != NAME_None)
	{
		TSharedPtr<IPropertyHandle> PropHandle = DetailLayout.GetProperty(CurrentPropertyName);
		check(PropHandle.IsValid());


		IDetailCategoryBuilder& DetailCategoryBuilder = DetailLayout.EditCategory("Property Detail");
		DetailCategoryBuilder.AddProperty(PropHandle);
	}
}


//virtual FText GetAssetDescription(const FAssetData& AssetData) const override;