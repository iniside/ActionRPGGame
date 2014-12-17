// Copyright 1998-2014 Epic Games, Inc. All Rights Reserved.
#pragma once
#include "TimeOfDayEditor.h"

#include "UnrealEd.h"
#include "Factories.h"
//
//#include "AssetRegistryModule.h"
//#include "Editor/ContentBrowser/Public/ContentBrowserModule.h"
//#include "Editor/ClassViewer/Public/ClassViewerModule.h"
//#include "Editor/ClassViewer/Public/ClassViewerFilter.h"
//#include "SClassPickerDialog.h"
//#include "MessageLog.h"
//#include "EnumEditorUtils.h"
//#include "StructureEditorUtils.h"
//
//#include "Serialization/Json/Json.h"
//
//#include "Editor/MainFrame/Public/MainFrame.h"
//
//#include "Runtime/JsonUtilities/Public/JsonUtilities.h"

#include "TODAssetFactory.h"

UTODAssetFactory::UTODAssetFactory(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{
	bCreateNew = true;
	bEditAfterNew = true;


	SupportedClass = UTODAsset::StaticClass();

}

bool UTODAssetFactory::ConfigureProperties()
{
	return true;
}
UObject* UTODAssetFactory::FactoryCreateNew(UClass* Class, UObject* InParent, FName Name, EObjectFlags Flags, UObject* Context, FFeedbackContext* Warn)
{
	if (ItemsDataClass != nullptr)
	{
		return CastChecked<UTODAsset>(StaticConstructObject(ItemsDataClass, InParent, Name, Flags | RF_Transactional));
	}
	else
	{
		// if we have no data asset class, use the passed-in class instead
		check(Class->IsChildOf(UTODAsset::StaticClass()));
		return CastChecked<UTODAsset>(StaticConstructObject(Class, InParent, Name, Flags));
	}
}

