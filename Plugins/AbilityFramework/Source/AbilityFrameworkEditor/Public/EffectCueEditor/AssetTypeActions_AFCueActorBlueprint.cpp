// Copyright 1998-2014 Epic Games, Inc. All Rights Reserved.
#include "AbilityFrameworkEditor.h"
#include "AssetTypeActions_AFCueStaticBlueprint.h"
#include "Misc/MessageDialog.h"
#include "Kismet2/BlueprintEditorUtils.h"
#include "GAEffectCueEditor.h"
#include "AFCueActorBlueprint.h"
#include "Effects/AFCueActor.h"
#include "AFCueActorBlueprintFactory.h"

#define LOCTEXT_NAMESPACE "AssetTypeActions"

FText FAssetTypeActions_AFCueActorBlueprint::GetName() const 
{ 
	return FText::FromString("Cue Actor");
}
UClass* FAssetTypeActions_AFCueActorBlueprint::GetSupportedClass() const
{
	return UAFCueActorBlueprint::StaticClass();
}

void FAssetTypeActions_AFCueActorBlueprint::OpenAssetEditor(const TArray<UObject*>& InObjects, TSharedPtr<IToolkitHost> EditWithinLevelEditor)
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

bool FAssetTypeActions_AFCueActorBlueprint::ShouldUseDataOnlyEditor(const UBlueprint* Blueprint) const
{
	return false;
}

UFactory* FAssetTypeActions_AFCueActorBlueprint::GetFactoryForBlueprintType(UBlueprint* InBlueprint) const
{
	UAFCueActorBlueprintFactory* EffectCueBlueprintFactory = NewObject<UAFCueActorBlueprintFactory>();
	EffectCueBlueprintFactory->ParentClass = TSubclassOf<AAFCueActor>(*InBlueprint->GeneratedClass);
	return EffectCueBlueprintFactory;
}

#undef LOCTEXT_NAMESPACE