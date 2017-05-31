// Copyright 1998-2014 Epic Games, Inc. All Rights Reserved.

#include "AbilityFrameworkEditor.h"
#include "Editor/PropertyEditor/Public/PropertyEditing.h"

#include "STextCombobox.h"
#include "STreeView.h"

#include "Effects/GAGameEffect.h"
#include "GAEffectDetails.h"
TSharedRef<IDetailCustomization> FGAEffectDetails::MakeInstance()
{
	return MakeShareable(new FGAEffectDetails);
}

void FGAEffectDetails::CustomizeDetails(IDetailLayoutBuilder& DetailLayout)
{
	MyDetailLayout = &DetailLayout;

	TArray<TWeakObjectPtr<UObject>> Objects;
	DetailLayout.GetObjectsBeingCustomized(Objects);

	/*TSharedPtr<IPropertyHandle> EffectTypeProp = DetailLayout.GetProperty(GET_MEMBER_NAME_CHECKED(UGAGameEffectSpec, EffectType), UGAGameEffectSpec::StaticClass());
	FSimpleDelegate UpdateEffectTypeyDelegate = FSimpleDelegate::CreateSP(this, &FGAEffectDetails::OnDurationPolicyChange);
	EffectTypeProp->SetOnPropertyValueChanged(UpdateEffectTypeyDelegate);*/

	/*for (TWeakObjectPtr<UObject> obj : Objects)
	{
		if (UGAGameEffectSpec* Spec = Cast<UGAGameEffectSpec>(obj.Get()))
		{
			bool bWithPeriod = Spec->bWithPeriod;
			switch (Spec->EffectType)
			{
			case EGAEffectType::Duration:
			{
				TSharedPtr<IPropertyHandle> PeriodicStackingProp = DetailLayout.GetProperty(GET_MEMBER_NAME_CHECKED(UGAGameEffectSpec, Stacking), UGAGameEffectSpec::StaticClass());
				TSharedPtr<IPropertyHandle> PeriodProp = DetailLayout.GetProperty(GET_MEMBER_NAME_CHECKED(UGAGameEffectSpec, Period), UGAGameEffectSpec::StaticClass());

				DetailLayout.HideProperty(PeriodicStackingProp);
				break;
			}
			case EGAEffectType::Infinite:
			{
				TSharedPtr<IPropertyHandle> DurationStackingProp = DetailLayout.GetProperty(GET_MEMBER_NAME_CHECKED(UGAGameEffectSpec, Stacking), UGAGameEffectSpec::StaticClass());
				
				TSharedPtr<IPropertyHandle> DurationProp = DetailLayout.GetProperty(GET_MEMBER_NAME_CHECKED(UGAGameEffectSpec, Duration), UGAGameEffectSpec::StaticClass());
				TSharedPtr<IPropertyHandle> PeriodProp = DetailLayout.GetProperty(GET_MEMBER_NAME_CHECKED(UGAGameEffectSpec, Period), UGAGameEffectSpec::StaticClass());
				TSharedPtr<IPropertyHandle> MaxStackedDurationProp = DetailLayout.GetProperty(GET_MEMBER_NAME_CHECKED(UGAGameEffectSpec, MaxStackedDuration), UGAGameEffectSpec::StaticClass());

				DetailLayout.HideProperty(DurationStackingProp);
				DetailLayout.HideProperty(DurationProp);
				DetailLayout.HideProperty(MaxStackedDurationProp);
				break;
			}
			case EGAEffectType::Instant:
			{
				TSharedPtr<IPropertyHandle> DurationStackingProp = DetailLayout.GetProperty(GET_MEMBER_NAME_CHECKED(UGAGameEffectSpec, Stacking), UGAGameEffectSpec::StaticClass());
				TSharedPtr<IPropertyHandle> MaxStacksProp = DetailLayout.GetProperty(GET_MEMBER_NAME_CHECKED(UGAGameEffectSpec, MaxStacks), UGAGameEffectSpec::StaticClass());
				TSharedPtr<IPropertyHandle> DurationProp = DetailLayout.GetProperty(GET_MEMBER_NAME_CHECKED(UGAGameEffectSpec, Duration), UGAGameEffectSpec::StaticClass());
				TSharedPtr<IPropertyHandle> PeriodProp = DetailLayout.GetProperty(GET_MEMBER_NAME_CHECKED(UGAGameEffectSpec, Period), UGAGameEffectSpec::StaticClass());
				TSharedPtr<IPropertyHandle> MaxStackedDurationProp = DetailLayout.GetProperty(GET_MEMBER_NAME_CHECKED(UGAGameEffectSpec, MaxStackedDuration), UGAGameEffectSpec::StaticClass());

				DetailLayout.HideProperty(DurationStackingProp);
				DetailLayout.HideProperty(MaxStacksProp);
				DetailLayout.HideProperty(DurationProp);
				DetailLayout.HideProperty(PeriodProp);
				DetailLayout.HideProperty(MaxStackedDurationProp);
				break;
			}
			}
		}
	}*/
}
void FGAEffectDetails::OnDurationPolicyChange()
{
	MyDetailLayout->ForceRefreshDetails();
}