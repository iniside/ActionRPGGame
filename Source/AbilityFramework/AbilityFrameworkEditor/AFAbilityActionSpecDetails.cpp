// Copyright 1998-2014 Epic Games, Inc. All Rights Reserved.

#include "AbilityFrameworkEditor.h"
#include "Editor/PropertyEditor/Public/PropertyEditing.h"

#include "STextCombobox.h"
#include "STreeView.h"

#include "Abilities/AFAbilityActivationSpec.h"
#include "AFAbilityActionSpecDetails.h"
#include "Effects/AFEffectCustomApplication.h"
TSharedRef<IDetailCustomization> FAFAbilityActionSpecDetails::MakeInstance()
{
	return MakeShareable(new FAFAbilityActionSpecDetails);
}

void FAFAbilityActionSpecDetails::CustomizeDetails(IDetailLayoutBuilder& DetailLayout)
{
	MyDetailLayout = &DetailLayout;

	TArray<TWeakObjectPtr<UObject>> Objects;
	DetailLayout.GetObjectsBeingCustomized(Objects);

	ApplicationTypeHandle = DetailLayout.GetProperty(GET_MEMBER_NAME_CHECKED(UGAGameEffectSpec, Application), UGAGameEffectSpec::StaticClass());
	FSimpleDelegate UpdateEffectTypeyDelegate = FSimpleDelegate::CreateSP(this, &FAFAbilityActionSpecDetails::OnDurationPolicyChange);
	ApplicationTypeHandle->SetOnPropertyValueChanged(UpdateEffectTypeyDelegate);

	for (TWeakObjectPtr<UObject> obj : Objects)
	{
		if (UAFAbilityActivationSpec* Spec = Cast<UAFAbilityActivationSpec>(obj.Get()))
		{
			bIsDuration = Spec->Application.GetDefaultObject()->ShowDuration();
			bIsPeriodic = Spec->Application.GetDefaultObject()->ShowPeriod();

			TSharedPtr<IPropertyHandle> EffectAggregationProp = DetailLayout.GetProperty(GET_MEMBER_NAME_CHECKED(UGAGameEffectSpec, EffectAggregation), UGAGameEffectSpec::StaticClass());
			DetailLayout.HideProperty(EffectAggregationProp);
			TSharedPtr<IPropertyHandle> MaxStackedDurationProp = DetailLayout.GetProperty(GET_MEMBER_NAME_CHECKED(UGAGameEffectSpec, MaxStackedDuration), UGAGameEffectSpec::StaticClass());
			DetailLayout.HideProperty(MaxStackedDurationProp);
			TSharedPtr<IPropertyHandle> MaxStacksProp = DetailLayout.GetProperty(GET_MEMBER_NAME_CHECKED(UGAGameEffectSpec, MaxStacks), UGAGameEffectSpec::StaticClass());
			DetailLayout.HideProperty(MaxStacksProp);

			TSharedPtr<IPropertyHandle> ExtensionProp = DetailLayout.GetProperty(GET_MEMBER_NAME_CHECKED(UGAGameEffectSpec, Extension), UGAGameEffectSpec::StaticClass());
			DetailLayout.HideProperty(ExtensionProp);
			TSharedPtr<IPropertyHandle> OnAppliedEffectsProp = DetailLayout.GetProperty(GET_MEMBER_NAME_CHECKED(UGAGameEffectSpec, OnAppliedEffects), UGAGameEffectSpec::StaticClass());
			DetailLayout.HideProperty(OnAppliedEffectsProp);
			TSharedPtr<IPropertyHandle> OnExpiredEffectsProp = DetailLayout.GetProperty(GET_MEMBER_NAME_CHECKED(UGAGameEffectSpec, OnExpiredEffects), UGAGameEffectSpec::StaticClass());
			DetailLayout.HideProperty(OnExpiredEffectsProp);
			TSharedPtr<IPropertyHandle> OnRemovedEffectsProp = DetailLayout.GetProperty(GET_MEMBER_NAME_CHECKED(UGAGameEffectSpec, OnRemovedEffects), UGAGameEffectSpec::StaticClass());
			DetailLayout.HideProperty(OnRemovedEffectsProp);

			DurationProperty = DetailLayout.GetProperty(GET_MEMBER_NAME_CHECKED(UGAGameEffectSpec, Duration), UGAGameEffectSpec::StaticClass());
			PeriodProperty = DetailLayout.GetProperty(GET_MEMBER_NAME_CHECKED(UGAGameEffectSpec, Period), UGAGameEffectSpec::StaticClass());

			if (bIsPeriodic && bIsDuration)
			{

			}
			else if (!bIsPeriodic && bIsDuration)
			{
				TSharedPtr<IPropertyHandle> PeriodProp = DetailLayout.GetProperty(GET_MEMBER_NAME_CHECKED(UGAGameEffectSpec, Period), UGAGameEffectSpec::StaticClass());
				DetailLayout.HideProperty(PeriodProp);
				
			}
			else if (bIsPeriodic && !bIsDuration)
			{
				TSharedPtr<IPropertyHandle> DurationProp = DetailLayout.GetProperty(GET_MEMBER_NAME_CHECKED(UGAGameEffectSpec, Duration), UGAGameEffectSpec::StaticClass());
				
				DetailLayout.HideProperty(DurationProp);
			}
			else if (!bIsPeriodic && !bIsDuration)
			{
				
				TSharedPtr<IPropertyHandle> DurationProp = DetailLayout.GetProperty(GET_MEMBER_NAME_CHECKED(UGAGameEffectSpec, Duration), UGAGameEffectSpec::StaticClass());
				TSharedPtr<IPropertyHandle> PeriodProp = DetailLayout.GetProperty(GET_MEMBER_NAME_CHECKED(UGAGameEffectSpec, Period), UGAGameEffectSpec::StaticClass());
				
				DetailLayout.HideProperty(DurationProp);
				DetailLayout.HideProperty(PeriodProp);
			}
		}
	}
}
void FAFAbilityActionSpecDetails::OnDurationPolicyChange()
{
	MyDetailLayout->ForceRefreshDetails();
}