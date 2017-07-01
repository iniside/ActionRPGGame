// Copyright 1998-2014 Epic Games, Inc. All Rights Reserved.
#include "../AbilityFrameworkEditor.h"
#include "AssetTypeActions_GAEffectBlueprint.h"
#include "Misc/MessageDialog.h"
#include "Kismet2/BlueprintEditorUtils.h"
#include "GAEffectEditor.h"
#include "Effects/GAEffectBlueprint.h"
#include "Effects/GAGameEffect.h"
#include "GAEffectBlueprintFactory.h"

#define LOCTEXT_NAMESPACE "AssetTypeActions"

FText FAssetTypeActions_GAEffectBlueprint::GetName() const 
{ 
	return FText::FromString("Effect Data");
}
UClass* FAssetTypeActions_GAEffectBlueprint::GetSupportedClass() const
{
	return UGAEffectBlueprint::StaticClass();
}

void FAssetTypeActions_GAEffectBlueprint::OpenAssetEditor(const TArray<UObject*>& InObjects, TSharedPtr<IToolkitHost> EditWithinLevelEditor)
{
	EToolkitMode::Type Mode = EditWithinLevelEditor.IsValid() ? EToolkitMode::WorldCentric : EToolkitMode::Standalone;

	for (auto ObjIt = InObjects.CreateConstIterator(); ObjIt; ++ObjIt)
	{
		auto Blueprint = Cast<UBlueprint>(*ObjIt);
		if (Blueprint && Blueprint->SkeletonGeneratedClass && Blueprint->GeneratedClass)
		{
			TSharedRef< FGAEffectEditor > NewEditor(new FGAEffectEditor());

			TArray<UBlueprint*> Blueprints;
			Blueprints.Add(Blueprint);

			NewEditor->InitEffectEditor(Mode, EditWithinLevelEditor, Blueprints, ShouldUseDataOnlyEditor(Blueprint));
		}
		else
		{
			FMessageDialog::Open(EAppMsgType::Ok, FText::FromString("Gameplay Ability Blueprint could not be loaded because it derives from an invalid class.  Check to make sure the parent class for this blueprint hasn't been removed!"));
		}
	}
}

bool FAssetTypeActions_GAEffectBlueprint::ShouldUseDataOnlyEditor(const UBlueprint* Blueprint) const
{
	return true;
}

UFactory* FAssetTypeActions_GAEffectBlueprint::GetFactoryForBlueprintType(UBlueprint* InBlueprint) const
{
	UGAEffectBlueprintFactory* EffectBlueprintFactory = NewObject<UGAEffectBlueprintFactory>();
	EffectBlueprintFactory->ParentClass = TSubclassOf<UGAGameEffectSpec>(*InBlueprint->GeneratedClass);
	return EffectBlueprintFactory;
}

#undef LOCTEXT_NAMESPACE