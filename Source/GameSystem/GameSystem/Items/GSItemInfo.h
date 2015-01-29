#pragma once
#include "GISItemData.h"
#include "GSItemTypes.h"
#include "GSItemInfo.generated.h"
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


UCLASS(BlueprintType, Blueprintable)
class GAMESYSTEM_API UGSItemInfo : public UGISItemData
{
	GENERATED_UCLASS_BODY()
public:
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Info")
		float Durability;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Info")
		float BasePrice;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Item Attributes")
		TArray<FGSItemAttributes> ItemAttributes;

	UPROPERTY(BlueprintReadWrite, meta = (ExposeOnSpawn))
		APawn* OwningPawn;

	UPROPERTY(BlueprintReadWrite, meta = (ExposeOnSpawn))
		APlayerController* OwiningPlayerController;

	virtual UTexture2D* GetImage() override;

	//these should be called on server.
	virtual bool OnItemAddedToSlot() override;
	virtual bool OnItemRemovedFromSlot() override;

	virtual bool CanItemBeSwapped() override;

	static const int32 ItemTypeID = 20;

	virtual int32 GetItemTypeID() const override { return UGSItemInfo::ItemTypeID; }

	virtual bool IsOfType(int32 ItemTypeIDIn) override { return UGSItemInfo::ItemTypeID == ItemTypeIDIn; }
};
