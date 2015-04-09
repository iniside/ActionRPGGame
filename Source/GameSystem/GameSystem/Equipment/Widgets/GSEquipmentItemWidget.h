#pragma once
#include "UserWidget.h"
#include "GSEquipmentItemWidget.generated.h"

UCLASS(BlueprintType, Blueprintable)
class GAMESYSTEM_API UGSEquipmentItemWidget : public UUserWidget
{
	GENERATED_BODY()
public:
	UGSEquipmentItemWidget(const FObjectInitializer& ObjectInitializer);
};
