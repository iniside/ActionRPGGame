#pragma once
#include "GSItemTypes.h"
#include "GSItem.generated.h"
/*
	Base class containing informations about items. Like trash items, 
	equipment, etc.

	1. Contain item attributes (possibly ?).
*/
UCLASS(BlueprintType, Blueprintable, DefaultToInstanced)
class GAMESYSTEM_API UGSItem : public UObject
{
	GENERATED_UCLASS_BODY()
public:
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Item Attributes")
		TArray<FGSItemAttributes> ItemAttributes;

	UPROPERTY(EditAnywhere, Category = "Info")
		UTexture2D* Icon;

	UPROPERTY(EditAnywhere, Category = "Info")
		USkeletalMesh* MySkeletalMesh;

	/*
		Lets item is weapon.
		UObject on it's own doesn't have anything needed make weapon work.
		Replication, rendering in world etc.

		So This class would contain weapon class for UGSItem_Weapon
		and when this item is equiped this class would be spawned, attached etc.
		makes sense ?
	*/
	UPROPERTY(EditAnywhere, Category = "Item Action")
		TSubclassOf<AActor> ItemActionClass;
};
