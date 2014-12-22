#pragma once
#include "GISItemData.h"
#include "GSAbilityInfo.generated.h"
/*
	Base class to show abilities in UI.
*/
UCLASS(BlueprintType, Blueprintable, DefaultToInstanced)
class GAMESYSTEM_API UGSAbilityInfo : public UGISItemData
{
	GENERATED_UCLASS_BODY()
public:
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Replicated)
		TSubclassOf<class AGSAbility> AbilityType;

	//needed ?
	UPROPERTY(Replicated)
	class AGSAbility* ActiveAbility;

	//these should be called on server.
	virtual bool OnItemAddedToSlot() override;
	virtual bool OnItemRemovedFromSlot() override;
	virtual void ActivateItem() override;

	static const int32 ItemTypeID = 10;

	virtual int32 GetItemTypeID() const { return UGSAbilityInfo::ItemTypeID; }

	virtual bool IsOfType(int32 ItemTypeIDIn) { return UGSAbilityInfo::ItemTypeID == ItemTypeIDIn; }
};
