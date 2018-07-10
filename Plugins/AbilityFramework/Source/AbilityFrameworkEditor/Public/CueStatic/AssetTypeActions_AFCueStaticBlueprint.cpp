// Copyright 1998-2014 Epic Games, Inc. All Rights Reserved.
#include "AbilityFrameworkEditor.h"
#include "AssetTypeActions_AFCueStaticBlueprint.h"
#include "Misc/MessageDialog.h"
#include "Kismet2/BlueprintEditorUtils.h"
#include "BlueprintEditor.h"
//#include "GAEffectCueEditor.h"
#include "Cues/AFCueStaticBlueprint.h"

#include "Effects/AFCueStatic.h"
#include "CueStatic/AFCueStaticBlueprintFactory.h"

#define LOCTEXT_NAMESPACE "AssetTypeActions"

FText FAssetTypeActions_AFCueStaticBlueprint::GetName() const 
{ 
	return FText::FromString("Cue Static");
}
UClass* FAssetTypeActions_AFCueStaticBlueprint::GetSupportedClass() const
{
	return UAFCueStaticBlueprint::StaticClass();
}

void FAssetTypeActions_AFCueStaticBlueprint::OpenAssetEditor(const TArray<UObject*>& InObjects, TSharedPtr<IToolkitHost> EditWithinLevelEditor)
{
	EToolkitMode::Type Mode = EditWithinLevelEditor.IsValid() ? EToolkitMode::WorldCentric : EToolkitMode::Standalone;

	for (auto ObjIt = InObjects.CreateConstIterator(); ObjIt; ++ObjIt)
	{
		auto Blueprint = Cast<UBlueprint>(*ObjIt);
		if (Blueprint && Blueprint->SkeletonGeneratedClass && Blueprint->GeneratedClass)
		{
			TSharedRef< FBlueprintEditor > NewEditor(new FBlueprintEditor());

			TArray<UBlueprint*> Blueprints;
			Blueprints.Add(Blueprint);

			NewEditor->InitBlueprintEditor(Mode, EditWithinLevelEditor, Blueprints, false);
		}
		else
		{
			FMessageDialog::Open(EAppMsgType::Ok, FText::FromString("Ability Blueprint could not be loaded because it derives from an invalid class.  Check to make sure the parent class for this blueprint hasn't been removed!"));
		}
	}
}

bool FAssetTypeActions_AFCueStaticBlueprint::ShouldUseDataOnlyEditor(const UBlueprint* Blueprint) const
{
	return false;
}

UFactory* FAssetTypeActions_AFCueStaticBlueprint::GetFactoryForBlueprintType(UBlueprint* InBlueprint) const
{
	UAFCueStaticBlueprintFactory* EffectCueBlueprintFactory = NewObject<UAFCueStaticBlueprintFactory>();
	EffectCueBlueprintFactory->ParentClass = TSubclassOf<UAFCueStatic>(*InBlueprint->GeneratedClass);
	return EffectCueBlueprintFactory;
}

#undef LOCTEXT_NAMESPACE