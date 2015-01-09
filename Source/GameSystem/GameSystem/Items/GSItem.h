#pragma once
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
	UPROPERTY(EditAnywhere, Category = "Info")
		UTexture2D* Icon;

	UPROPERTY(EditAnywhere, Category = "Info")
		USkeletalMesh* MySkeletalMesh;
};
