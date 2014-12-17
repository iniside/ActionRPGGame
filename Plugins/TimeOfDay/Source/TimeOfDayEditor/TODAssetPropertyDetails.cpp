// Copyright 1998-2014 Epic Games, Inc. All Rights Reserved.

#include "TimeOfDayEditor.h"

#include "TODAssetPropertyDetails.h"

float FTODFloatCurveProperty::GetCurrentFloatValue() const
{
	if (PropertyHandle.IsValid())
	{
		UProperty* propPtr = PropertyHandle->GetProperty();
		UStructProperty* structProp = Cast<UStructProperty>(propPtr);
		if (structProp)
		{
			FRuntimeFloatCurve* floatCurve = structProp->ContainerPtrToValuePtr<FRuntimeFloatCurve>(TODAsset);
			if (floatCurve)
			{
				return floatCurve->EditorCurveData.Eval(TODAsset->Hour);
			}
		}
	}
	return 0;
}

FReply FTODFloatCurveProperty::HandleOnButtonClicked(TSharedPtr<IPropertyHandle> PropertyHandle)
{
	if (OnPropertySelected.IsBound())
	{
		if (PropertyHandle.IsValid())
		{
			FName test = PropertyHandle->GetProperty()->GetFName();
			OnPropertySelected.Execute(PropertyHandle->GetProperty());
		}
	}
	return FReply::Unhandled();
}
float FTODFloatCurveProperty::GetFloatValueFromAttribute() const
{
	return GetFloatAttribute.Get();
}

void FTODFloatCurveProperty::OnFloatValueChanged(float ValueIn)
{
	if (OnFloatCurveValueChanged.IsBound())
	{
		OnFloatCurveValueChanged.Execute(ValueIn);
	}
}
void FTODFloatCurveProperty::ConstructWidget(IDetailCategoryBuilder& CategoryBuilder)
{
	CategoryBuilder.AddCustomRow(FString("Test Row"))
		[
			SNew(SButton)
			.ForegroundColor(FSlateColor::UseForeground())
			.ButtonStyle(FEditorStyle::Get(), "HoverHintOnly")
			.OnClicked(this, &FTODFloatCurveProperty::HandleOnButtonClicked, PropertyHandle)
			[
				SNew(SProperty, PropertyHandle)
				.CustomWidget()
				[
					SNew(SHorizontalBox)
					+ SHorizontalBox::Slot()
					[
						SNew(SVerticalBox)
						+ SVerticalBox::Slot()
							[
								SNew(SSpinBox<float>)
								.Delta(0.2)
								.MinValue(0)
								.MaxValue(1024)
								.MinSliderValue(0)
								.MaxSliderValue(1024)
								.Value(TAttribute<float>(this, &FTODFloatCurveProperty::GetFloatValueFromAttribute))
								.OnValueChanged(this, &FTODFloatCurveProperty::OnFloatValueChanged)
							]
					]
				]
			]
		];
}


FLinearColor FTODColorCurveProperty::GetCurrentColorValue() const
{
	FLinearColor returnColor = FLinearColor(0, 0, 0, 0);
	if (PropertyHandle.IsValid())
	{
		UProperty* propPtr = PropertyHandle->GetProperty();
		UStructProperty* structProp = Cast<UStructProperty>(propPtr);
		if (structProp)
		{
			FRuntimeCurveLinearColor* colorCurve = structProp->ContainerPtrToValuePtr<FRuntimeCurveLinearColor>(TODAsset);
			if (colorCurve && colorCurve->IsOfType(FRuntimeCurveLinearColor::CurveTypeID))
			{
				returnColor = colorCurve->GetLinearColorValue(TODAsset->Hour);
				return returnColor;
			}
		}
	}
	return returnColor;
}

FReply FTODColorCurveProperty::HandleOnButtonClicked(TSharedPtr<IPropertyHandle> PropertyHandle)
{
	if (OnPropertySelected.IsBound())
	{
		if (PropertyHandle.IsValid())
		{
			OnPropertySelected.Execute(PropertyHandle->GetProperty());
		}
	}
	return FReply::Unhandled();
}
FLinearColor FTODColorCurveProperty::GetColorValueFromAttribute() const
{
	return GetColorAttribute.Get();
}

void FTODColorCurveProperty::ColorPicker_OnColorCommitted(FLinearColor InColor)
{
	if (OnColorCurveValueChanged.IsBound())
	{
		OnColorCurveValueChanged.Execute(InColor);
	}
}

FReply FTODColorCurveProperty::ColorBlock_OnMouseButtonDown(const FGeometry& MyGeometry, const FPointerEvent& MouseEvent)
{
	if (MouseEvent.GetEffectingButton() == EKeys::LeftMouseButton)
	{
		FLinearColor currentColor = GetColorAttribute.Get();
		FColorPickerArgs ColorPickerArgs;
		TArray<FLinearColor*> LinearColorArray;
		LinearColorArray.Add(&currentColor);
		ColorPickerArgs.LinearColorArray = &LinearColorArray;
		ColorPickerArgs.OnColorCommitted = FOnLinearColorValueChanged::CreateSP(this, &FTODColorCurveProperty::ColorPicker_OnColorCommitted);

		OpenColorPicker(ColorPickerArgs);

		return FReply::Handled();
	}

	return FReply::Unhandled();
}

void FTODColorCurveProperty::ConstructWidget(IDetailCategoryBuilder& CategoryBuilder)
{
	CategoryBuilder.AddCustomRow(FString("Test Row"))
		[
			SNew(SButton)
			.ForegroundColor(FSlateColor::UseForeground())
			.ButtonStyle(FEditorStyle::Get(), "HoverHintOnly")
			.OnClicked(this, &FTODColorCurveProperty::HandleOnButtonClicked, PropertyHandle)
			[
				SNew(SProperty, PropertyHandle)
				.CustomWidget()
				[
					SNew(SHorizontalBox)
					+ SHorizontalBox::Slot()
					[
						SNew(SVerticalBox)
						+ SVerticalBox::Slot()
						[
							SNew(SColorBlock)
							.UseSRGB(true)
							.ColorIsHSV(false)
							.Color(TAttribute<FLinearColor>(this, &FTODColorCurveProperty::GetColorValueFromAttribute))
							.OnMouseButtonDown(this, &FTODColorCurveProperty::ColorBlock_OnMouseButtonDown)
						]
					]
				]
			]
		];
}



TSharedRef<IDetailCustomization> FTODAssetPropertyDetails::MakeInstance(FTODOnPropertySelected OnPropertySelectedIn, FTODOnFloatCurveValueChanged OnFloatCurveValueChangedIn, FTODOnColorCurveValueChanged OnColorCurveValueChangedIn)
{
	return MakeShareable(new FTODAssetPropertyDetails(OnPropertySelectedIn, OnFloatCurveValueChangedIn, OnColorCurveValueChangedIn));
}

FTODAssetPropertyDetails::~FTODAssetPropertyDetails()
{
	for (TSharedPtr<FTODFloatCurveProperty> floatCurve : FloatCurves)
	{
		floatCurve.Reset();
	}
	FloatCurves.Empty();
}

void FTODAssetPropertyDetails::CustomizeDetails(IDetailLayoutBuilder& DetailLayout)
{
	const IDetailsView& DetailView = DetailLayout.GetDetailsView();
	//first find asset we are going to edit.
	TWeakObjectPtr<UObject> InspectedObject;
	for (TWeakObjectPtr<UObject> inspObj : DetailView.GetSelectedObjects())
	{
		InspectedObject = inspObj;
		break;
	}

	UTODAsset* TODAsset = Cast<UTODAsset>(InspectedObject.Get());
	CurrentTODAsset = Cast<UTODAsset>(InspectedObject.Get());
	if (TODAsset)
	{
		for (TFieldIterator<UProperty> PropIt(TODAsset->GetClass()); PropIt; ++PropIt)
		{
			UProperty* prop = *PropIt;
			DetailLayout.HideProperty(prop->GetFName());
			//PropertyHandles.Add(DetailLayout.GetProperty(prop->GetFName()));
			UStructProperty* structProp = Cast<UStructProperty>(prop);
			if (structProp)
			{
				FRuntimeFloatCurve* floatCurve = structProp->ContainerPtrToValuePtr<FRuntimeFloatCurve>(TODAsset);
				if (floatCurve && floatCurve->IsOfType(FRuntimeFloatCurve::CurveTypeID))
				{
					TSharedPtr<FTODFloatCurveProperty> tempFloatProp = MakeShareable(new FTODFloatCurveProperty());
					tempFloatProp->OnPropertySelected = OnPropertySelected;
					tempFloatProp->OnFloatCurveValueChanged = OnFloatCurveValueChanged;
					tempFloatProp->PropertyHandle = DetailLayout.GetProperty(prop->GetFName());
					tempFloatProp->TODAsset = TODAsset;
					tempFloatProp->CategoryName = tempFloatProp->PropertyHandle->GetMetaData(TEXT("Category"));
					FloatCurves.Add(tempFloatProp);
				}
			}
		}
		for (TFieldIterator<UProperty> PropIt(TODAsset->GetClass()); PropIt; ++PropIt)
		{
			UProperty* prop = *PropIt;
			DetailLayout.HideProperty(prop->GetFName());
			//PropertyHandles.Add(DetailLayout.GetProperty(prop->GetFName()));
			UStructProperty* structProp = Cast<UStructProperty>(prop);
			if (structProp)
			{
				FRuntimeCurveLinearColor* colorCurve = structProp->ContainerPtrToValuePtr<FRuntimeCurveLinearColor>(TODAsset);
				if (colorCurve && colorCurve->IsOfType(FRuntimeCurveLinearColor::CurveTypeID))
				{
					TSharedPtr<FTODColorCurveProperty> tempColorProp = MakeShareable(new FTODColorCurveProperty());
					tempColorProp->OnPropertySelected = OnPropertySelected;
					tempColorProp->OnColorCurveValueChanged = OnColorCurveValueChanged;
					tempColorProp->PropertyHandle = DetailLayout.GetProperty(prop->GetFName());
					tempColorProp->TODAsset = TODAsset;
					tempColorProp->CategoryName = tempColorProp->PropertyHandle->GetMetaData(TEXT("Category"));
					ColorCurves.Add(tempColorProp);
				}
			}

		}
	}
	IDetailCategoryBuilder& DetailCategoryBuilder = DetailLayout.EditCategory("Property Detail");
	FDetailWidgetRow& DetailRow = DetailCategoryBuilder.AddCustomRow(FString("Custom Row"));
	////now customize each property
	//FRuntimeFloatCurve* floatCurve;

	TSharedPtr<IPropertyHandle> hour = DetailLayout.GetProperty(TEXT("Hour"));
	DetailCategoryBuilder.AddProperty(hour);


	IDetailCategoryBuilder& SunCategoryBuilder = DetailLayout.EditCategory("Sun");
	IDetailCategoryBuilder& AFCategoryBuilder = DetailLayout.EditCategory("Atmospheric Fog");
	IDetailCategoryBuilder& HFCategoryBuilder = DetailLayout.EditCategory("Height Fog");
	IDetailCategoryBuilder& PPCategoryBuilder = DetailLayout.EditCategory("Post Process");
	IDetailCategoryBuilder& SkyLightCategoryBuilder = DetailLayout.EditCategory("SkyLight");
	IDetailCategoryBuilder& MoonCategoryBuilder = DetailLayout.EditCategory("Moon");

	//probabaly not he best way around, but it works pretty nice.
	//and it's not like we will be reconstructing it every second in anyway.
	for (TSharedPtr<FTODFloatCurveProperty> floatCurves : FloatCurves)
	{
		if (floatCurves->CategoryName == FString("Sun"))
			floatCurves->ConstructWidget(SunCategoryBuilder);

		if (floatCurves->CategoryName == FString("Atmospheric Fog"))
			floatCurves->ConstructWidget(AFCategoryBuilder);

		if (floatCurves->CategoryName == FString("Height Fog"))
			floatCurves->ConstructWidget(HFCategoryBuilder);

		if (floatCurves->CategoryName == FString("Post Process"))
			floatCurves->ConstructWidget(PPCategoryBuilder);

		if (floatCurves->CategoryName == FString("SkyLight"))
			floatCurves->ConstructWidget(SkyLightCategoryBuilder);

		if (floatCurves->CategoryName == FString("Moon"))
			floatCurves->ConstructWidget(MoonCategoryBuilder);
	}

	for (TSharedPtr<FTODColorCurveProperty> colorCurves : ColorCurves)
	{
		if (colorCurves->CategoryName == FString("Sun"))
			colorCurves->ConstructWidget(SunCategoryBuilder);

		if (colorCurves->CategoryName == FString("Atmospheric Fog"))
			colorCurves->ConstructWidget(AFCategoryBuilder);

		if (colorCurves->CategoryName == FString("Height Fog"))
			colorCurves->ConstructWidget(HFCategoryBuilder);

		if (colorCurves->CategoryName == FString("Post Process"))
			colorCurves->ConstructWidget(PPCategoryBuilder);

		if (colorCurves->CategoryName == FString("SkyLight"))
			colorCurves->ConstructWidget(SkyLightCategoryBuilder);

		if (colorCurves->CategoryName == FString("Moon"))
			colorCurves->ConstructWidget(MoonCategoryBuilder);
	}
}
//accept propetty name ?
float FTODAssetPropertyDetails::GetCurrentFloatValue() const
{
	if (SunIntensityCurve.IsValid())
	{
		UProperty* propPtr = SunIntensityCurve->GetProperty();
		UStructProperty* structProp = Cast<UStructProperty>(propPtr);
		FRuntimeFloatCurve* floatCurve = structProp->ContainerPtrToValuePtr<FRuntimeFloatCurve>(CurrentTODAsset);
		if (floatCurve)
		{
			return floatCurve->EditorCurveData.Eval(CurrentTODAsset->Hour);
		}
	}
	return 0;
}
float FTODAssetPropertyDetails::GetSunIntensityValue() const
{
	return GetSunIntensityCurveValue.Get();
}
TSharedPtr<SProperty> FTODAssetPropertyDetails::GetPropertyWidget(TSharedPtr<IPropertyHandle> PropertyHandleIn)
{
	return SNew(SProperty, PropertyHandleIn);
}

float FTODAssetPropertyDetails::GetFloatCurveValue(FRuntimeFloatCurve* FloatCurveIn, float TimeIn)
{
	return FloatCurveIn->EditorCurveData.Eval(TimeIn);
}
//virtual FText GetAssetDescription(const FAssetData& AssetData) const override;