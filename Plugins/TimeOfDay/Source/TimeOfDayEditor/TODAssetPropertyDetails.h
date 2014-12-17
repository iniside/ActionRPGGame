#pragma once
#include "PropertyEditing.h"
#include "PropertyCustomizationHelpers.h"
#include "AssetTypeActions_Base.h"

#include "SColorPicker.h"

#include "Editor/PropertyEditor/Public/IDetailCustomNodeBuilder.h"

DECLARE_DELEGATE_RetVal(FName, FTODOnGetSelectedFloatCurveName);

DECLARE_DELEGATE_OneParam(FTODOnPropertySelected, UProperty*);

DECLARE_DELEGATE_OneParam(FTODOnFloatCurveValueChanged, float);

DECLARE_DELEGATE_OneParam(FTODOnColorCurveValueChanged, FLinearColor);

/*
	Customization for curve property display.

	Instead of displaying Curve, it will display name of property and edit box with value of
	of curve in current time.
	

	If there is no key in current value and value will be changed, new key will be added to curve
	at current time. -- not working yet!
*/
class FTODProperty
{
public:
	FTODProperty()
	{
		//CategoryBuilder = nullptr;
	}
	TSharedPtr<IPropertyHandle> PropertyHandle;

	TAttribute<float> GetFloatAttribute;
	TAttribute<FLinearColor> GetColorAttribute;
	UTODAsset* TODAsset;

	FString CategoryName;
};
class FTODFloatCurveProperty : public FTODProperty, public TSharedFromThis<FTODFloatCurveProperty>
{
public:
	FTODOnPropertySelected OnPropertySelected;
	FTODOnFloatCurveValueChanged OnFloatCurveValueChanged;

	float GetCurrentFloatValue() const;

	float GetFloatValueFromAttribute() const;


	void ConstructWidget(IDetailCategoryBuilder& CategoryBuilder);

	FReply HandleOnButtonClicked(TSharedPtr<IPropertyHandle> PropertyHandle);

	void OnFloatValueChanged(float ValueIn);
	FTODFloatCurveProperty()
	{

		//FloatCurve = nullptr;
		TODAsset = nullptr;
		GetFloatAttribute.BindRaw(this, &FTODFloatCurveProperty::GetCurrentFloatValue);
	}

	~FTODFloatCurveProperty()
	{
		//FloatCurve = nullptr;
		TODAsset = nullptr;
	}
};


class FTODColorCurveProperty : public FTODProperty, public TSharedFromThis<FTODColorCurveProperty>
{
public:
	FTODOnPropertySelected OnPropertySelected;
	FTODOnColorCurveValueChanged OnColorCurveValueChanged;
	FLinearColor GetCurrentColorValue() const;

	FLinearColor GetColorValueFromAttribute() const;

	void ConstructWidget(IDetailCategoryBuilder& CategoryBuilder);

	FReply HandleOnButtonClicked(TSharedPtr<IPropertyHandle> PropertyHandle);
	FReply ColorBlock_OnMouseButtonDown(const FGeometry& MyGeometry, const FPointerEvent& MouseEvent);

	void ColorPicker_OnColorCommitted(FLinearColor InColor);

	FTODColorCurveProperty()
	{

		//FloatCurve = nullptr;
		TODAsset = nullptr;
		GetColorAttribute.BindRaw(this, &FTODColorCurveProperty::GetCurrentColorValue);
	}

	~FTODColorCurveProperty()
	{
		//FloatCurve = nullptr;
		TODAsset = nullptr;
	}
};

//add another class for color curve.


class FTODAssetPropertyDetails : public IDetailCustomization
{
public:
	FTODAssetPropertyDetails(FTODOnPropertySelected OnPropertySelectedIn, FTODOnFloatCurveValueChanged OnFloatCurveValueChangedIn
		, FTODOnColorCurveValueChanged OnColorCurveValueChangedIn)
		: OnPropertySelected(OnPropertySelectedIn),
		OnFloatCurveValueChanged(OnFloatCurveValueChangedIn),
		OnColorCurveValueChanged(OnColorCurveValueChangedIn)
	{};
	~FTODAssetPropertyDetails();
	/** Makes a new instance of this detail layout class for a specific detail view requesting it */
	static TSharedRef<IDetailCustomization> MakeInstance(FTODOnPropertySelected OnPropertySelectedIn, FTODOnFloatCurveValueChanged OnFloatCurveValueChangedIn
		, FTODOnColorCurveValueChanged OnColorCurveValueChangedIn);

	/** IDetailCustomization interface */
	virtual void CustomizeDetails(IDetailLayoutBuilder& DetailLayout) override;

protected:
	FTODOnGetSelectedFloatCurveName OnGetSelectedFloatCurveName;
	FTODOnPropertySelected OnPropertySelected;

	FTODOnFloatCurveValueChanged OnFloatCurveValueChanged;

	FTODOnColorCurveValueChanged OnColorCurveValueChanged;

	TArray<TSharedPtr<IPropertyHandle>> PropertyHandles;

	TArray<TSharedPtr<FTODFloatCurveProperty>> FloatCurves;
	TArray<TSharedPtr<FTODColorCurveProperty>> ColorCurves;
	TSharedPtr<IPropertyHandle> SunIntensityCurve;
	TAttribute<float> GetSunIntensityCurveValue;

	TSharedPtr<SProperty> GetPropertyWidget(TSharedPtr<IPropertyHandle> PropertyHandleIn);

	float GetFloatCurveValue(FRuntimeFloatCurve* FloatCurveIn, float TimeIn);

	float GetSunIntensityValue() const;

	float GetCurrentFloatValue() const;

	class UTODAsset* CurrentTODAsset;
};