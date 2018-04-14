#pragma once
#include "CoreMinimal.h"
#include "Toolkits/IToolkitHost.h"
#include "AssetTypeCategories.h"
//#include "Developer/AssetTools/Private/AssetTypeActions/AssetTypeActions_ClassTypeBase.h"
#include "Developer/AssetTools/Public/AssetTypeActions/AssetTypeActions_Blueprint.h"

class UFactory;

class FAssetTypeActions_GAEffectBlueprint : public FAssetTypeActions_Blueprint
{
public:
	// IAssetTypeActions Implementation
	virtual FText GetName() const override;
	virtual FColor GetTypeColor() const override { return FColor(0, 96, 128); }
	virtual UClass* GetSupportedClass() const override;
	virtual void OpenAssetEditor(const TArray<UObject*>& InObjects, TSharedPtr<class IToolkitHost> EditWithinLevelEditor = TSharedPtr<IToolkitHost>()) override;
	virtual uint32 GetCategories() override { return EAssetTypeCategories::Gameplay; }
	// End IAssetTypeActions Implementation

	// FAssetTypeActions_Blueprint interface
	virtual UFactory* GetFactoryForBlueprintType(UBlueprint* InBlueprint) const override;

private:
	/** Returns true if the blueprint is data only */
	bool ShouldUseDataOnlyEditor(const UBlueprint* Blueprint) const;
};