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
	UPROPERTY(BlueprintReadWrite, meta=(ExposeOnSpawn), Replicated)
		TSubclassOf<class AGSAbility> AbilityType;

	UPROPERTY(BlueprintReadWrite, meta = (ExposeOnSpawn))
		APawn* OwningPawn;

	UPROPERTY(BlueprintReadWrite, meta = (ExposeOnSpawn))
		APlayerController* OwiningPlayerController;

	//needed ?
	UPROPERTY(Replicated)
	class AGSAbility* ActiveAbility;

	virtual UTexture2D* GetImage() override;

	//these should be called on server.
	virtual bool OnItemAddedToSlot() override;
	virtual bool OnItemRemovedFromSlot() override;

	inline const float GetCurrentCooldownTime() const
	{
		if (ActiveAbility)
		{
			return ActiveAbility->GetCurrentCooldownTime();
		}
		return 0;
	}

	inline const float GetCooldownTime() const
	{
		if (ActiveAbility)
		{
			return ActiveAbility->GetCooldownTime();
		}
		return 0;
	}

	virtual bool InputPressed_Implementation() override;
	virtual bool InputReleased_Implementation() override;

	virtual bool CanItemBeSwapped() override;

	static const int32 ItemTypeID = 10;

	virtual int32 GetItemTypeID() const { return UGSAbilityInfo::ItemTypeID; }

	virtual bool IsOfType(int32 ItemTypeIDIn) { return UGSAbilityInfo::ItemTypeID == ItemTypeIDIn; }
};
