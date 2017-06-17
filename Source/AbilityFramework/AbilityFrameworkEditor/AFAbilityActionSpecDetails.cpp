// Copyright 1998-2014 Epic Games, Inc. All Rights Reserved.

#include "AbilityFrameworkEditor.h"
#include "Editor/PropertyEditor/Public/PropertyEditing.h"

#include "STextCombobox.h"
#include "STreeView.h"

#include "Abilities/AFAbilityActivationSpec.h"
#include "AFAbilityActionSpecDetails.h"
#include "Effects/AFEffectCustomApplication.h"
#include "AFEffectCustomizationCommon.h"
#include "IPropertyUtilities.h"

TSharedRef<IDetailCustomization> FAFAbilityActivationSpecDetails::MakeInstance()
{
	return MakeShareable(new FAFAbilityActivationSpecDetails);
}

void FAFAbilityActivationSpecDetails::CustomizeDetails(IDetailLayoutBuilder& DetailLayout)
{
	TArray<TWeakObjectPtr<UObject>> Objects;
	DetailLayout.GetObjectsBeingCustomized(Objects);
	if (Objects.Num() != 1)
	{
		// I don't know what to do here or what could be expected. Just return to disable all templating functionality
		return;
	}
	ApplicationTypeHandle = DetailLayout.GetProperty(GET_MEMBER_NAME_CHECKED(UGAGameEffectSpec, Application), UGAGameEffectSpec::StaticClass());
	FSimpleDelegate UpdateEffectTypeyDelegate = FSimpleDelegate::CreateSP(this, &FAFAbilityActivationSpecDetails::OnDurationPolicyChange);
	ApplicationTypeHandle->SetOnPropertyValueChanged(UpdateEffectTypeyDelegate);

	for (TWeakObjectPtr<UObject> obj : Objects)
	{
		if (UAFAbilityActivationSpec* Spec = Cast<UAFAbilityActivationSpec>(obj.Get()))
		{
			MyDetailLayout = MakeShareable(&DetailLayout);

			FAFEffectCustomizationCommon::HideProperty(DetailLayout, "ApplicationRequirement");
			//FAFEffectCustomizationCommon::HideProperty(DetailLayout, "Application");

			FAFEffectCustomizationCommon::HideProperty(DetailLayout, "EffectAggregation");
			FAFEffectCustomizationCommon::HideProperty(DetailLayout, "MaxStackedDuration");
			FAFEffectCustomizationCommon::HideProperty(DetailLayout, "MaxStacks");
			FAFEffectCustomizationCommon::HideProperty(DetailLayout, "bTickOnApplication");
			FAFEffectCustomizationCommon::HideProperty(DetailLayout, "bExecuteOnApplication");

			FAFEffectCustomizationCommon::HideProperty(DetailLayout, "Extension");
			FAFEffectCustomizationCommon::HideProperty(DetailLayout, "OnAppliedEffects");
			FAFEffectCustomizationCommon::HideProperty(DetailLayout, "OnExpiredEffects");
			FAFEffectCustomizationCommon::HideProperty(DetailLayout, "OnRemovedEffects");
			FAFEffectCustomizationCommon::HideProperty(DetailLayout, "ConditonalEffects");
			FAFEffectCustomizationCommon::HideProperty(DetailLayout, "RemoveEffectWithTags");
			FAFEffectCustomizationCommon::HideProperty(DetailLayout, "AtributeModifier");
			FAFEffectCustomizationCommon::HideProperty(DetailLayout, "Modifiers");

			FAFEffectCustomizationCommon::HideProperty(DetailLayout, "AppliedEventTags");
			FAFEffectCustomizationCommon::HideProperty(DetailLayout, "ExecuteEventTags");
			FAFEffectCustomizationCommon::HideProperty(DetailLayout, "AttributeTags");
			FAFEffectCustomizationCommon::HideProperty(DetailLayout, "DenyTags");
			FAFEffectCustomizationCommon::HideProperty(DetailLayout, "AppliedImmunityTags");
			FAFEffectCustomizationCommon::HideProperty(DetailLayout, "RequiredTags");
			FAFEffectCustomizationCommon::HideProperty(DetailLayout, "ExecutionRequiredTags");

			TSharedPtr<IPropertyHandle> DurationProperty = DetailLayout.GetProperty(GET_MEMBER_NAME_CHECKED(UGAGameEffectSpec, Duration), UGAGameEffectSpec::StaticClass());
			TSharedPtr<IPropertyHandle> PeriodProperty = DetailLayout.GetProperty(GET_MEMBER_NAME_CHECKED(UGAGameEffectSpec, Period), UGAGameEffectSpec::StaticClass());

			DetailLayout.HideProperty(DurationProperty);
			DetailLayout.HideProperty(PeriodProperty);
			DurationCalcTypeProp = DurationProperty->GetChildHandle("CalculationType");
			FAFEffectCustomizationCommon::CreateMagnitudeLayout(DetailLayout, DurationProperty, "Duration");
			DurationCalcTypeProp->SetOnPropertyValueChanged(UpdateEffectTypeyDelegate);
		}
	}
}
void FAFAbilityActivationSpecDetails::OnDurationPolicyChange()
{
	if (MyDetailLayout.IsValid())
	{
		MyDetailLayout->ForceRefreshDetails();
		//const TSharedRef< class IPropertyUtilities >& util = MyDetailLayout->GetPropertyUtilities();
		//util.Get().RequestRefresh();
	}
}