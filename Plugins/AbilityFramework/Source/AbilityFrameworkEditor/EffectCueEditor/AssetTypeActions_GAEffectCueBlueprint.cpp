// Copyright 1998-2014 Epic Games, Inc. All Rights Reserved.
#include "../AbilityFrameworkEditor.h"
#include "AssetTypeActions_GAEffectCueBlueprint.h"
#include "Misc/MessageDialog.h"
#include "Kismet2/BlueprintEditorUtils.h"
#include "GAEffectCueEditor.h"
#include "GAEffectCueBlueprint.h"
#include "Effects/GAEffectCue.h"
#include "GAEffectCueBlueprintFactory.h"

#define LOCTEXT_NAMESPACE "AssetTypeActions"

FText FAssetTypeActions_GAEffectCueBlueprint::GetName() const 
{ 
	return FText::FromString("Effect Cue");
}
UClass* FAssetTypeActions_GAEffectCueBlueprint::GetSupportedClass() const
{
	return UGAEffectCueBlueprint::StaticClass();
}

void FAssetTypeActions_GAEffectCueBlueprint::OpenAssetEditor(const TArray<UObject*>& InObjects, TSharedPtr<IToolkitHost> EditWithinLevelEditor)
{
	EToolkitMode::Type Mode = EditWithinLevelEditor.IsValid() ? EToolkitMode::WorldCentric : EToolkitMode::Standalone;

	for (auto ObjIt = InObjects.CreateConstIterator(); ObjIt; ++ObjIt)
	{
		auto Blueprint = Cast<UBlueprint>(*ObjIt);
		if (Blueprint && Blueprint->SkeletonGeneratedClass && Blueprint->GeneratedClass)
		{
			TSharedRef< FGAEffectCueEditor > NewEditor(new FGAEffectCueEditor());

			TArray<UBlueprint*> Blueprints;
			Blueprints.Add(Blueprint);

			NewEditor->InitEffectCueEditor(Mode, EditWithinLevelEditor, Blueprints, ShouldUseDataOnlyEditor(Blueprint));
		}
		else
		{
			FMessageDialog::Open(EAppMsgType::Ok, FText::FromString("Ability Blueprint could not be loaded because it derives from an invalid class.  Check to make sure the parent class for this blueprint hasn't been removed!"));
		}
	}
}

bool FAssetTypeActions_GAEffectCueBlueprint::ShouldUseDataOnlyEditor(const UBlueprint* Blueprint) const
{
	return false;
}

UFactory* FAssetTypeActions_GAEffectCueBlueprint::GetFactoryForBlueprintType(UBlueprint* InBlueprint) const
{
	UGAEffectCueBlueprintFactory* EffectCueBlueprintFactory = NewObject<UGAEffectCueBlueprintFactory>();
	EffectCueBlueprintFactory->ParentClass = TSubclassOf<AGAEffectCue>(*InBlueprint->GeneratedClass);
	return EffectCueBlueprintFactory;
}

#undef LOCTEXT_NAMESPACE