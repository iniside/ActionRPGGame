// Copyright 1998-2014 Epic Games, Inc. All Rights Reserved.
#include "../AbilityFrameworkEditor.h"
#include "AssetTypeActions_GAAbilityBlueprint.h"
#include "Misc/MessageDialog.h"
#include "Kismet2/BlueprintEditorUtils.h"
#include "GAAbilityEditor.h"
#include "Abilities/GAAbilityBlueprint.h"
#include "Abilities/GAAbilityBase.h"
#include "GAAbilityBlueprintFactory.h"

#define LOCTEXT_NAMESPACE "AssetTypeActions"

FText FAssetTypeActions_GAAbilityBlueprint::GetName() const 
{ 
	return FText::FromString("Ability");
}
UClass* FAssetTypeActions_GAAbilityBlueprint::GetSupportedClass() const
{
	return UGAAbilityBlueprint::StaticClass();
}

void FAssetTypeActions_GAAbilityBlueprint::OpenAssetEditor(const TArray<UObject*>& InObjects, TSharedPtr<IToolkitHost> EditWithinLevelEditor)
{
	EToolkitMode::Type Mode = EditWithinLevelEditor.IsValid() ? EToolkitMode::WorldCentric : EToolkitMode::Standalone;

	for (auto ObjIt = InObjects.CreateConstIterator(); ObjIt; ++ObjIt)
	{
		auto Blueprint = Cast<UBlueprint>(*ObjIt);
		if (Blueprint && Blueprint->SkeletonGeneratedClass && Blueprint->GeneratedClass)
		{
			TSharedRef< FGAAbilityEditor > NewEditor(new FGAAbilityEditor());

			TArray<UBlueprint*> Blueprints;
			Blueprints.Add(Blueprint);

			NewEditor->InitAbilitiesEditor(Mode, EditWithinLevelEditor, Blueprints, ShouldUseDataOnlyEditor(Blueprint));
		}
		else
		{
			FMessageDialog::Open(EAppMsgType::Ok, FText::FromString("Ability Blueprint could not be loaded because it derives from an invalid class.  Check to make sure the parent class for this blueprint hasn't been removed!"));
		}
	}
}

bool FAssetTypeActions_GAAbilityBlueprint::ShouldUseDataOnlyEditor(const UBlueprint* Blueprint) const
{
	return false;
}

UFactory* FAssetTypeActions_GAAbilityBlueprint::GetFactoryForBlueprintType(UBlueprint* InBlueprint) const
{
	UGAAbilityBlueprintFactory* AbilityBlueprintFactory = NewObject<UGAAbilityBlueprintFactory>();
	AbilityBlueprintFactory->ParentClass = TSubclassOf<UGAAbilityBase>(*InBlueprint->GeneratedClass);
	return AbilityBlueprintFactory;
}

#undef LOCTEXT_NAMESPACE