#pragma once
#include "GSItemTypes.h"
#include "GSItemInfo.h"
#include "GSItemCharacterEquipmentInfo.generated.h"
/*
	1. Should I add attributes here ?
	2. Should this be purerly inventory cosmetic class ?
	3. Or should it contain information about actuall item ? Like attributes,
	meshes, references to other items (for composite items, which are assmbled from various
	componenets).
*/
/*
	Temporary.
*/


UCLASS(BlueprintType, Blueprintable, DefaultToInstanced)
class GAMESYSTEM_API UGSItemCharacterEquipmentInfo : public UGSItemInfo
{
	GENERATED_UCLASS_BODY()
public:

	/*
		Main item.
	*/
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Replicated, meta = (ExposeOnSpawn), Category = "Item")
		TSubclassOf<class UGSItem> Item;
	/*
		Item Componenets. These can be added to Item.
	*/
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Replicated, meta = (ExposeOnSpawn), Category = "Item")
		TArray<TSubclassOf<class UGSItem>> ItemParts;

	virtual UTexture2D* GetImage() override;

	//these should be called on server.
	virtual bool OnItemAddedToSlot() override;
	virtual bool OnItemRemovedFromSlot() override;

	virtual bool CanItemBeSwapped() override;

	static const int32 ItemTypeID = 40;

	virtual int32 GetItemTypeID() const override { return UGSItemCharacterEquipmentInfo::ItemTypeID; }

	virtual bool IsOfType(int32 ItemTypeIDIn) override { return UGSItemCharacterEquipmentInfo::ItemTypeID == ItemTypeIDIn; }
};
