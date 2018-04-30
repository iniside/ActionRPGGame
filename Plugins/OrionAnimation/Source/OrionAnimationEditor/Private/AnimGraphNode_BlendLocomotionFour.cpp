// Copyright 2018 Sean Chen. All Rights Reserved.

#include "AnimGraphNode_BlendLocomotionFour.h"
#include "OrionAnimationEditor.h"

#define LOCTEXT_NAMESPACE "A3Nodes"

UAnimGraphNode_BlendLocomotionFour::UAnimGraphNode_BlendLocomotionFour()
{

}

FLinearColor UAnimGraphNode_BlendLocomotionFour::GetNodeTitleColor() const
{
	return FLinearColor(0.7f, 0.7f, 0.7f);
}

FText UAnimGraphNode_BlendLocomotionFour::GetTooltipText() const
{
	return LOCTEXT("LocomotionFour", "Locomotion Four Direction");
}

FText UAnimGraphNode_BlendLocomotionFour::GetNodeTitle(ENodeTitleType::Type TitleType) const
{
	return LOCTEXT("LocomotionFour", "Locomotion Four Direction");
}

FString UAnimGraphNode_BlendLocomotionFour::GetNodeCategory() const
{
	return TEXT("Orion Animation");
}
void GetPinInformation(const FString& InPinName, int32& Out_PinIndex, bool& Out_bIsPosePin, bool& Out_bIsTimePin)
{
	const int32 UnderscoreIndex = InPinName.Find(TEXT("_"), ESearchCase::CaseSensitive);
	if (UnderscoreIndex != INDEX_NONE)
	{
		const FString ArrayName = InPinName.Left(UnderscoreIndex);
		Out_PinIndex = FCString::Atoi(*(InPinName.Mid(UnderscoreIndex + 1)));

		Out_bIsPosePin = (ArrayName == TEXT("BlendPose"));
		Out_bIsTimePin = (ArrayName == TEXT("BlendTime"));
	}
	else
	{
		Out_bIsPosePin = false;
		Out_bIsTimePin = false;
		Out_PinIndex = INDEX_NONE;
	}
}
void UAnimGraphNode_BlendLocomotionFour::CustomizePinData(UEdGraphPin* Pin, FName SourcePropertyName, int32 ArrayIndex) const
{
	// if pin name starts with BlendPose or BlendWeight, change to enum name 
	bool bIsPosePin;
	bool bIsTimePin;
	int32 RawArrayIndex;
	GetPinInformation(Pin->PinName.ToString(), /*out*/ RawArrayIndex, /*out*/ bIsPosePin, /*out*/ bIsTimePin);
	checkSlow(RawArrayIndex == ArrayIndex);


	if (bIsPosePin)
	{
		if (RawArrayIndex == 0)
		{
			//Pin->PinFriendlyName = FText::FromName("North Pose");
			//FFormatNamedArguments Args;
			//Args.Add(TEXT("PinFriendlyName"), Pin->PinFriendlyName);
			Pin->PinFriendlyName = FText::FromString("N Pose");
		}
		else if (RawArrayIndex == 1)
		{
			Pin->PinFriendlyName = FText::FromString("E Pose");
		}
		else if (RawArrayIndex == 2)
		{
			Pin->PinFriendlyName = FText::FromString("S Pose");
		}
		else if (RawArrayIndex == 3)
		{
			Pin->PinFriendlyName = FText::FromString("W Pose");
		}
	}
	//if (bIsPosePin || bIsTimePin)
	//{
	//	if (RawArrayIndex > 0)
	//	{
	//		const int32 ExposedEnumPinIndex = RawArrayIndex - 1;

	//		// find pose index and see if it's mapped already or not
	//		if (VisibleEnumEntries.IsValidIndex(ExposedEnumPinIndex) && (BoundEnum != NULL))
	//		{
	//			const FName& EnumElementName = VisibleEnumEntries[ExposedEnumPinIndex];
	//			const int32 EnumIndex = BoundEnum->GetIndexByName(EnumElementName);
	//			if (EnumIndex != INDEX_NONE)
	//			{
	//				Pin->PinFriendlyName = BoundEnum->GetDisplayNameTextByIndex(EnumIndex);
	//			}
	//			else
	//			{
	//				Pin->PinFriendlyName = FText::FromName(EnumElementName);
	//			}
	//		}
	//		else
	//		{
	//			Pin->PinFriendlyName = LOCTEXT("InvalidIndex", "Invalid index");
	//		}
	//	}
	//	else if (ensure(RawArrayIndex == 0))
	//	{
	//		Pin->PinFriendlyName = LOCTEXT("Default", "Default");
	//	}

	//	// Append the pin type
	//	if (bIsPosePin)
	//	{
	//		FFormatNamedArguments Args;
	//		Args.Add(TEXT("PinFriendlyName"), Pin->PinFriendlyName);
	//		Pin->PinFriendlyName = FText::Format(LOCTEXT("FriendlyNamePose", "{PinFriendlyName} Pose"), Args);
	//	}

	//	if (bIsTimePin)
	//	{
	//		FFormatNamedArguments Args;
	//		Args.Add(TEXT("PinFriendlyName"), Pin->PinFriendlyName);
	//		Pin->PinFriendlyName = FText::Format(LOCTEXT("FriendlyNameBlendTime", "{PinFriendlyName} Blend Time"), Args);
	//	}
	//}
}

#undef LOCTEXT_NAMESPACE

