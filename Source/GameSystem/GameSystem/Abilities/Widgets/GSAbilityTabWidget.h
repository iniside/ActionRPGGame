#pragma once
#include "UserWidget.h"
#include "GSAbilityTabWidget.generated.h"

UCLASS(BlueprintType, Blueprintable)
class GAMESYSTEM_API UGSAbilityTabWidget : public UUserWidget
{
	GENERATED_UCLASS_BODY()
public:
	UPROPERTY(BlueprintReadOnly, Category = "UI")
		TArray<class UGSAbilitySlotWidget*> AbilitySlots;
};
