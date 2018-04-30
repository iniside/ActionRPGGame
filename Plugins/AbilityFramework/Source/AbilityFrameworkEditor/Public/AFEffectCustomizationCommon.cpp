// Fill out your copyright notice in the Description page of Project Settings.

#include "AbilityFrameworkEditor.h"
#include "Effects/GAGameEffect.h"
#include "DetailLayoutBuilder.h"
#include "DetailCategoryBuilder.h"
#include "IDetailGroup.h"
#include "AFEffectCustomizationCommon.h"

FAFEffectCustomizationCommon::FAFEffectCustomizationCommon()
{
}

FAFEffectCustomizationCommon::~FAFEffectCustomizationCommon()
{
}
void FAFEffectCustomizationCommon::CreateMagnitudeLayout(IDetailLayoutBuilder& DetailLayout,
	TSharedPtr<IPropertyHandle>& InProperty, FName InCategory)
{
	IDetailCategoryBuilder& DurationCategory = DetailLayout.EditCategory(InCategory);

	TSharedPtr<IPropertyHandle> DurationCalcTypeProp = InProperty->GetChildHandle("CalculationType");
	
	uint8 CalcType = 0;
	DurationCalcTypeProp->GetValue(CalcType);

	TSharedPtr<IPropertyHandle> DirectModifier = InProperty->GetChildHandle("DirectModifier");
	TSharedPtr<IPropertyHandle> AttributeBased = InProperty->GetChildHandle("AttributeBased");
	TSharedPtr<IPropertyHandle> CurveBased = InProperty->GetChildHandle("CurveBased");
	TSharedPtr<IPropertyHandle> Custom = InProperty->GetChildHandle("Custom");

	DurationCategory.AddProperty(DurationCalcTypeProp.ToSharedRef());
	switch (CalcType)
	{
	case 0: //Direct
	{
		DetailLayout.HideProperty(DirectModifier);
		DetailLayout.HideProperty(AttributeBased);
		DetailLayout.HideProperty(CurveBased);
		DetailLayout.HideProperty(Custom);
		
		TSharedPtr<IPropertyHandle> Value = DirectModifier->GetChildHandle("Value");
		DurationCategory.AddProperty(Value.ToSharedRef());
		//DetailGroup.
		break;
	}
	case 1: //AttributeBased
	{
		DetailLayout.HideProperty(DirectModifier);
		DetailLayout.HideProperty(AttributeBased);
		DetailLayout.HideProperty(CurveBased);
		DetailLayout.HideProperty(Custom);

		TSharedPtr<IPropertyHandle> Source = AttributeBased->GetChildHandle("Source");
		TSharedPtr<IPropertyHandle> Attribute = AttributeBased->GetChildHandle("Attribute");
		TSharedPtr<IPropertyHandle> Coefficient = AttributeBased->GetChildHandle("Coefficient");
		TSharedPtr<IPropertyHandle> PreMultiply = AttributeBased->GetChildHandle("PreMultiply");
		TSharedPtr<IPropertyHandle> PostMultiply = AttributeBased->GetChildHandle("PostMultiply");
		TSharedPtr<IPropertyHandle> PostCoefficient = AttributeBased->GetChildHandle("PostCoefficient");
		TSharedPtr<IPropertyHandle> bUseSecondaryAttribute = AttributeBased->GetChildHandle("bUseSecondaryAttribute");
		TSharedPtr<IPropertyHandle> SecondarySource = AttributeBased->GetChildHandle("SecondarySource");
		TSharedPtr<IPropertyHandle> SecondaryAttribute = AttributeBased->GetChildHandle("SecondaryAttribute");

		DurationCategory.AddProperty(Source.ToSharedRef());
		DurationCategory.AddProperty(Attribute.ToSharedRef());
		DurationCategory.AddProperty(Coefficient.ToSharedRef());
		DurationCategory.AddProperty(PreMultiply.ToSharedRef());
		DurationCategory.AddProperty(PostMultiply.ToSharedRef());
		DurationCategory.AddProperty(PostCoefficient.ToSharedRef());
		DurationCategory.AddProperty(bUseSecondaryAttribute.ToSharedRef());
		DurationCategory.AddProperty(SecondarySource.ToSharedRef());
		DurationCategory.AddProperty(SecondaryAttribute.ToSharedRef());

		break;
	}
	case 2: //CurveBased
	{
		DetailLayout.HideProperty(AttributeBased);
		DetailLayout.HideProperty(DirectModifier);
		DetailLayout.HideProperty(CurveBased);
		DetailLayout.HideProperty(Custom);

		TSharedPtr<IPropertyHandle> Source = CurveBased->GetChildHandle("Source");
		TSharedPtr<IPropertyHandle> Attribute = CurveBased->GetChildHandle("Attribute");
		TSharedPtr<IPropertyHandle> CurveTable = CurveBased->GetChildHandle("CurveTable");

		DurationCategory.AddProperty(Source.ToSharedRef());
		DurationCategory.AddProperty(Attribute.ToSharedRef());
		DurationCategory.AddProperty(CurveTable.ToSharedRef());
		break;
	}
	case 3: //CustomCalculation
	{
		DetailLayout.HideProperty(AttributeBased);
		DetailLayout.HideProperty(CurveBased);
		DetailLayout.HideProperty(DirectModifier);
		DetailLayout.HideProperty(Custom);
		
		TSharedPtr<IPropertyHandle> CustomCalculation = Custom->GetChildHandle("CustomCalculation");
		DurationCategory.AddProperty(CustomCalculation.ToSharedRef());
		break;
	}
	}
}
void FAFEffectCustomizationCommon::HideProperty(IDetailLayoutBuilder& DetailLayout, FName InName)
{
	UProperty* prop = UGAGameEffectSpec::StaticClass()->FindPropertyByName(InName);
	
	TSharedPtr<IPropertyHandle> Property = DetailLayout.GetProperty(InName, UGAGameEffectSpec::StaticClass());
	DetailLayout.HideProperty(Property);
}