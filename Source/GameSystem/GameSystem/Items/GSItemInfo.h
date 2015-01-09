#pragma once
#include "GISItemData.h"
#include "GSItemInfo.generated.h"
/*
	1. Should I add attributes here ?
	2. Should this be purerly inventory cosmetic class ?
	3. Or should it contain information about actuall item ? Like attributes,
	meshes, references to other items (for composite items, which are assmbled from various
	componenets).
*/
UCLASS(BlueprintType, Blueprintable, DefaultToInstanced)
class GAMESYSTEM_API UGSItemInfo : public UGISItemData
{
	GENERATED_UCLASS_BODY()
public:

	UPROPERTY(BlueprintReadWrite, Replicated, meta = (ExposeOnSpawn), Category = "Item")
		TSubclassOf<class UGSItem> Item;

	UPROPERTY(BlueprintReadWrite, meta = (ExposeOnSpawn))
		APawn* OwningPawn;

	UPROPERTY(BlueprintReadWrite, meta = (ExposeOnSpawn))
		APlayerController* OwiningPlayerController;

	virtual UTexture2D* GetImage() override;

	//these should be called on server.
	virtual bool OnItemAddedToSlot() override;
	virtual bool OnItemRemovedFromSlot() override;


	virtual void InputPressed() override;
	virtual void InputReleased() override;

	virtual bool CanItemBeSwapped() override;

	static const int32 ItemTypeID = 20;

	virtual int32 GetItemTypeID() const override { return UGSItemInfo::ItemTypeID; }

	virtual bool IsOfType(int32 ItemTypeIDIn) override { return UGSItemInfo::ItemTypeID == ItemTypeIDIn; }
};
