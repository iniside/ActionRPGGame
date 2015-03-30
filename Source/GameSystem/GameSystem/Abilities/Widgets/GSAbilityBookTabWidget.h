#pragma once
#include "UserWidget.h"
#include "GSAbilityBookTabWidget.generated.h"

UCLASS(BlueprintType, Blueprintable)
class GAMESYSTEM_API UGSAbilityBookTabWidget : public UUserWidget
{
	GENERATED_UCLASS_BODY()
public:
	UPROPERTY(BlueprintReadOnly, Category = "UI")
		TArray<class UGSAbilityBookSlotWidget*> TabSlots;
};
