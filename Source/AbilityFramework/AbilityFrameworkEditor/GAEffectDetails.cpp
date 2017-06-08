// Copyright 1998-2014 Epic Games, Inc. All Rights Reserved.

#include "AbilityFrameworkEditor.h"
#include "Editor/PropertyEditor/Public/PropertyEditing.h"

#include "STextCombobox.h"
#include "STreeView.h"

#include "Effects/GAGameEffect.h"
#include "GAEffectDetails.h"
#include "Effects/AFEffectCustomApplication.h"
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
	FSimpleDelegate UpdateEffectTypeyDelegate = FSimpleDelegate::CreateSP(this, &FGAEffectDetails::OnDurationPolicyChange);
	ApplicationTypeHandle->SetOnPropertyValueChanged(UpdateEffectTypeyDelegate);

	for (TWeakObjectPtr<UObject> obj : Objects)
	{
		if (UGAGameEffectSpec* Spec = Cast<UGAGameEffectSpec>(obj.Get()))
		{
			bIsDuration = Spec->Application.GetDefaultObject()->ShowDuration();
			bIsPeriodic = Spec->Application.GetDefaultObject()->ShowPeriod();

			if (bIsPeriodic && bIsDuration)
			{

			}
			else if (!bIsPeriodic && bIsDuration)
			{
				TSharedPtr<IPropertyHandle> MaxStacksProp = DetailLayout.GetProperty(GET_MEMBER_NAME_CHECKED(UGAGameEffectSpec, MaxStacks), UGAGameEffectSpec::StaticClass());
				TSharedPtr<IPropertyHandle> DurationProp = DetailLayout.GetProperty(GET_MEMBER_NAME_CHECKED(UGAGameEffectSpec, Duration), UGAGameEffectSpec::StaticClass());
				TSharedPtr<IPropertyHandle> PeriodProp = DetailLayout.GetProperty(GET_MEMBER_NAME_CHECKED(UGAGameEffectSpec, Period), UGAGameEffectSpec::StaticClass());
				TSharedPtr<IPropertyHandle> MaxStackedDurationProp = DetailLayout.GetProperty(GET_MEMBER_NAME_CHECKED(UGAGameEffectSpec, MaxStackedDuration), UGAGameEffectSpec::StaticClass());

				DetailLayout.HideProperty(MaxStacksProp);
				DetailLayout.HideProperty(PeriodProp);
				DetailLayout.HideProperty(MaxStackedDurationProp);
			}
			else if (bIsPeriodic && !bIsDuration)
			{
				TSharedPtr<IPropertyHandle> MaxStacksProp = DetailLayout.GetProperty(GET_MEMBER_NAME_CHECKED(UGAGameEffectSpec, MaxStacks), UGAGameEffectSpec::StaticClass());
				TSharedPtr<IPropertyHandle> DurationProp = DetailLayout.GetProperty(GET_MEMBER_NAME_CHECKED(UGAGameEffectSpec, Duration), UGAGameEffectSpec::StaticClass());
				TSharedPtr<IPropertyHandle> PeriodProp = DetailLayout.GetProperty(GET_MEMBER_NAME_CHECKED(UGAGameEffectSpec, Period), UGAGameEffectSpec::StaticClass());
				TSharedPtr<IPropertyHandle> MaxStackedDurationProp = DetailLayout.GetProperty(GET_MEMBER_NAME_CHECKED(UGAGameEffectSpec, MaxStackedDuration), UGAGameEffectSpec::StaticClass());

				DetailLayout.HideProperty(MaxStacksProp);
				DetailLayout.HideProperty(DurationProp);
				DetailLayout.HideProperty(MaxStackedDurationProp);
			}
			else if (!bIsPeriodic && !bIsDuration)
			{
				TSharedPtr<IPropertyHandle> MaxStacksProp = DetailLayout.GetProperty(GET_MEMBER_NAME_CHECKED(UGAGameEffectSpec, MaxStacks), UGAGameEffectSpec::StaticClass());
				TSharedPtr<IPropertyHandle> DurationProp = DetailLayout.GetProperty(GET_MEMBER_NAME_CHECKED(UGAGameEffectSpec, Duration), UGAGameEffectSpec::StaticClass());
				TSharedPtr<IPropertyHandle> PeriodProp = DetailLayout.GetProperty(GET_MEMBER_NAME_CHECKED(UGAGameEffectSpec, Period), UGAGameEffectSpec::StaticClass());
				TSharedPtr<IPropertyHandle> MaxStackedDurationProp = DetailLayout.GetProperty(GET_MEMBER_NAME_CHECKED(UGAGameEffectSpec, MaxStackedDuration), UGAGameEffectSpec::StaticClass());

				DetailLayout.HideProperty(MaxStacksProp);
				DetailLayout.HideProperty(DurationProp);
				DetailLayout.HideProperty(PeriodProp);
				DetailLayout.HideProperty(MaxStackedDurationProp);
			}
		}
	}
}
void FGAEffectDetails::OnDurationPolicyChange()
{
	MyDetailLayout->ForceRefreshDetails();
}