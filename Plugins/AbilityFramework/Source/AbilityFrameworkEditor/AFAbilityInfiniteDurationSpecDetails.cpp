// Copyright 1998-2014 Epic Games, Inc. All Rights Reserved.

#include "AbilityFrameworkEditor.h"
#include "Editor/PropertyEditor/Public/PropertyEditing.h"

#include "STextCombobox.h"
#include "STreeView.h"

#include "Abilities/AFAbilityInfiniteDurationSpec.h"
#include "AFAbilityInfiniteDurationSpecDetails.h"
#include "Effects/AFEffectCustomApplication.h"
#include "AFEffectCustomizationCommon.h"
TSharedRef<IDetailCustomization> FAFAbilityInfiniteDurationSpecDetails::MakeInstance()
{
	return MakeShareable(new FAFAbilityInfiniteDurationSpecDetails);
}

void FAFAbilityInfiniteDurationSpecDetails::CustomizeDetails(IDetailLayoutBuilder& DetailLayout)
{
	MyDetailLayout = &DetailLayout;

	TArray<TWeakObjectPtr<UObject>> Objects;
	DetailLayout.GetObjectsBeingCustomized(Objects);

	ApplicationTypeHandle = DetailLayout.GetProperty(GET_MEMBER_NAME_CHECKED(UGAGameEffectSpec, Application), UGAGameEffectSpec::StaticClass());
	FSimpleDelegate UpdateEffectTypeyDelegate = FSimpleDelegate::CreateSP(this, &FAFAbilityInfiniteDurationSpecDetails::OnDurationPolicyChange);
	ApplicationTypeHandle->SetOnPropertyValueChanged(UpdateEffectTypeyDelegate);

	for (TWeakObjectPtr<UObject> obj : Objects)
	{
		if (UAFAbilityInfiniteDurationSpec* Spec = Cast<UAFAbilityInfiniteDurationSpec>(obj.Get()))
		{
			bIsDuration = Spec->Application.GetDefaultObject()->ShowDuration();
			bIsPeriodic = Spec->Application.GetDefaultObject()->ShowPeriod();

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

			DurationProperty = DetailLayout.GetProperty(GET_MEMBER_NAME_CHECKED(UGAGameEffectSpec, Duration), UGAGameEffectSpec::StaticClass());
			PeriodProperty = DetailLayout.GetProperty(GET_MEMBER_NAME_CHECKED(UGAGameEffectSpec, Period), UGAGameEffectSpec::StaticClass());
			DetailLayout.HideCategory("Duration");
			DetailLayout.HideCategory("Period");

			DetailLayout.HideProperty(DurationProperty);
			DetailLayout.HideProperty(PeriodProperty);
			DurationCalcTypeProp = DurationProperty->GetChildHandle("CalculationType");
			PeriodCalcTypeProp = PeriodProperty->GetChildHandle("CalculationType");

			FAFEffectCustomizationCommon::CreateMagnitudeLayout(DetailLayout, DurationProperty, "Duration");
			FAFEffectCustomizationCommon::CreateMagnitudeLayout(DetailLayout, PeriodProperty, "Period");

			DurationCalcTypeProp->SetOnPropertyValueChanged(UpdateEffectTypeyDelegate);
			PeriodCalcTypeProp->SetOnPropertyValueChanged(UpdateEffectTypeyDelegate);
		}
	}
}
void FAFAbilityInfiniteDurationSpecDetails::OnDurationPolicyChange()
{
	MyDetailLayout->ForceRefreshDetails();
}