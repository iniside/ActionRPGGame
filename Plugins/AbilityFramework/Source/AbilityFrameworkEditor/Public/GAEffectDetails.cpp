// Copyright 1998-2014 Epic Games, Inc. All Rights Reserved.

#include "AbilityFrameworkEditor.h"
#include "Editor/PropertyEditor/Public/PropertyEditing.h"

#include "STextCombobox.h"
#include "STreeView.h"

#include "Effects/GAGameEffect.h"
#include "GAEffectDetails.h"
#include "Effects/AFEffectCustomApplication.h"
#include "AFEffectCustomizationCommon.h"
#include "Abilities/AFAbilityActivationSpec.h"
#include "Abilities/AFAbilityPeriodSpec.h"
#include "Abilities/AFAbilityCooldownSpec.h"
TSharedRef<IDetailCustomization> FGAEffectDetails::MakeInstance()
{
	return MakeShareable(new FGAEffectDetails);
}

void FGAEffectDetails::CustomizeDetails(IDetailLayoutBuilder& DetailLayout)
{
	MyDetailLayout = &DetailLayout;

	TArray<TWeakObjectPtr<UObject>> Objects;
	DetailLayout.GetObjectsBeingCustomized(Objects);

	ApplicationTypeHandle = DetailLayout.GetProperty(GET_MEMBER_NAME_CHECKED(UGAGameEffectSpec, Application), UGAGameEffectSpec::StaticClass());
	UpdateEffectTypeyDelegate = FSimpleDelegate::CreateSP(this, &FGAEffectDetails::OnDurationPolicyChange);
	ApplicationTypeHandle->SetOnPropertyValueChanged(UpdateEffectTypeyDelegate);

	DurationHandle = DetailLayout.GetProperty(GET_MEMBER_NAME_CHECKED(UGAGameEffectSpec, Duration), UGAGameEffectSpec::StaticClass());
	PeriodHandle = DetailLayout.GetProperty(GET_MEMBER_NAME_CHECKED(UGAGameEffectSpec, Period), UGAGameEffectSpec::StaticClass());
	DurationHandle->SetOnPropertyValueChanged(UpdateEffectTypeyDelegate);
	PeriodHandle->SetOnPropertyValueChanged(UpdateEffectTypeyDelegate);

	DurationCalcTypeHandle = DurationHandle->GetChildHandle("CalculationType");
	PeriodCalcTypeHandle = PeriodHandle->GetChildHandle("CalculationType");

	DurationCalcTypeHandle->SetOnPropertyValueChanged(UpdateEffectTypeyDelegate);
	PeriodCalcTypeHandle->SetOnPropertyValueChanged(UpdateEffectTypeyDelegate);

	for (TWeakObjectPtr<UObject> obj : Objects)
	{
		if (UAFEffectSpecBase* Spec = Cast<UAFEffectSpecBase>(obj.Get()))
		{
			/*if (!Spec->IsA(UAFAbilityActivationSpec::StaticClass())
				&& !Spec->IsA(UAFAbilityPeriodSpec::StaticClass())
				&& !Spec->IsA(UAFAbilityCooldownSpec::StaticClass())
				)*/
			{
				bIsDuration = Spec->Application.GetDefaultObject()->ShowDuration();
				bIsPeriodic = Spec->Application.GetDefaultObject()->ShowPeriod();

				if (bIsPeriodic && bIsDuration)
				{
					DetailLayout.HideProperty(PeriodHandle);
					DetailLayout.HideProperty(DurationHandle);
					FAFEffectCustomizationCommon::CreateMagnitudeLayout(DetailLayout, DurationHandle, "Duration");
					FAFEffectCustomizationCommon::CreateMagnitudeLayout(DetailLayout, PeriodHandle, "Period");
				}
				else if (!bIsPeriodic && bIsDuration)
				{
					TSharedPtr<IPropertyHandle> MaxStacksProp = DetailLayout.GetProperty(GET_MEMBER_NAME_CHECKED(UGAGameEffectSpec, MaxStacks), UGAGameEffectSpec::StaticClass());
					TSharedPtr<IPropertyHandle> MaxStackedDurationHandle = DetailLayout.GetProperty(GET_MEMBER_NAME_CHECKED(UGAGameEffectSpec, MaxStackedDuration), UGAGameEffectSpec::StaticClass());

					DetailLayout.HideProperty(MaxStacksProp);
					DetailLayout.HideProperty(PeriodHandle);
					DetailLayout.HideProperty(DurationHandle);
					DetailLayout.HideProperty(MaxStackedDurationHandle);

					FAFEffectCustomizationCommon::CreateMagnitudeLayout(DetailLayout, DurationHandle, "Duration");
				}
				else if (bIsPeriodic && !bIsDuration)
				{
					TSharedPtr<IPropertyHandle> MaxStacksProp = DetailLayout.GetProperty(GET_MEMBER_NAME_CHECKED(UGAGameEffectSpec, MaxStacks), UGAGameEffectSpec::StaticClass());
					TSharedPtr<IPropertyHandle> MaxStackedDurationHandle = DetailLayout.GetProperty(GET_MEMBER_NAME_CHECKED(UGAGameEffectSpec, MaxStackedDuration), UGAGameEffectSpec::StaticClass());

					DetailLayout.HideProperty(MaxStacksProp);
					DetailLayout.HideProperty(DurationHandle);
					DetailLayout.HideProperty(PeriodHandle);
					DetailLayout.HideProperty(MaxStackedDurationHandle);

					FAFEffectCustomizationCommon::CreateMagnitudeLayout(DetailLayout, PeriodHandle, "Period");
				}
				else if (!bIsPeriodic && !bIsDuration)
				{
					TSharedPtr<IPropertyHandle> MaxStacksProp = DetailLayout.GetProperty(GET_MEMBER_NAME_CHECKED(UGAGameEffectSpec, MaxStacks), UGAGameEffectSpec::StaticClass());
					TSharedPtr<IPropertyHandle> MaxStackedDurationHandle = DetailLayout.GetProperty(GET_MEMBER_NAME_CHECKED(UGAGameEffectSpec, MaxStackedDuration), UGAGameEffectSpec::StaticClass());

					DetailLayout.HideProperty(MaxStacksProp);
					DetailLayout.HideProperty(DurationHandle);
					DetailLayout.HideProperty(PeriodHandle);
					DetailLayout.HideProperty(MaxStackedDurationHandle);
					DetailLayout.HideCategory("Duration");
					FAFEffectCustomizationCommon::HideProperty(DetailLayout, "EffectAggregation");
					FAFEffectCustomizationCommon::HideProperty(DetailLayout, "OnExpiredEffects");
					FAFEffectCustomizationCommon::HideProperty(DetailLayout, "OnRemovedEffects");
					FAFEffectCustomizationCommon::HideProperty(DetailLayout, "AttributeTags");
					FAFEffectCustomizationCommon::HideProperty(DetailLayout, "AppliedImmunityTags");
					FAFEffectCustomizationCommon::HideProperty(DetailLayout, "ApplyTags");

				}
			}
		}
	}
}
void FGAEffectDetails::OnDurationPolicyChange()
{
	if(MyDetailLayout)
		MyDetailLayout->ForceRefreshDetails();
}