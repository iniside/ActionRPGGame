#include "WALandscapeGraphAssetTypeActions.h"
#include "WALandscapeGraphAssetEditor.h"

#define LOCTEXT_NAMESPACE "AssetTypeActions"

FWALandscapeGraphAssetTypeActions::FWALandscapeGraphAssetTypeActions(EAssetTypeCategories::Type InAssetCategory)
	: MyAssetCategory(InAssetCategory)
{
}

FText FWALandscapeGraphAssetTypeActions::GetName() const
{
	return FText::FromString("Landscape Graph");
}

FColor FWALandscapeGraphAssetTypeActions::GetTypeColor() const
{
	return FColor(129, 196, 115);
}

UClass* FWALandscapeGraphAssetTypeActions::GetSupportedClass() const
{
	return UWALandscapeGraph::StaticClass();
}

void FWALandscapeGraphAssetTypeActions::OpenAssetEditor(const TArray<UObject*>& InObjects, TSharedPtr<class IToolkitHost> EditWithinLevelEditor)
{
	const EToolkitMode::Type Mode = EditWithinLevelEditor.IsValid() ? EToolkitMode::WorldCentric : EToolkitMode::Standalone;

	for (auto ObjIt = InObjects.CreateConstIterator(); ObjIt; ++ObjIt)
	{
		if (UWALandscapeGraph* Graph = Cast<UWALandscapeGraph>(*ObjIt))
		{
			TSharedRef<FWALandscapeGraphAssetEditor> NewGraphEditor(new FWALandscapeGraphAssetEditor());
			NewGraphEditor->InitGenericGraphAssetEditor(Mode, EditWithinLevelEditor, Graph);
		}
	}
}

uint32 FWALandscapeGraphAssetTypeActions::GetCategories()
{
	return EAssetTypeCategories::Basic | MyAssetCategory;
}

//////////////////////////////////////////////////////////////////////////

#undef LOCTEXT_NAMESPACE