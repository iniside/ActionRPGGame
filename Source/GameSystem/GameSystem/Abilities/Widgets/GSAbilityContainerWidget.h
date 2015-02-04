#pragma once
#include "UserWidget.h"
#include "GSAbilityContainerWidget.generated.h"

UCLASS(BlueprintType, Blueprintable)
class GAMESYSTEM_API UGSAbilityContainerWidget : public UUserWidget
{
	GENERATED_UCLASS_BODY()
public:
	UPROPERTY()
		class UGSAbilitiesComponent* AbilityComponent;

	UPROPERTY()
		TSubclassOf<class UGSAbilityTabWidget> AbilityTabClass;

	UPROPERTY()
		TSubclassOf<class UGSAbilitySlotWidget> AbilitySlotClass;

	UPROPERTY()
		TSubclassOf<class UGSAbilityWidget> AbilityWidgetClass;

	UPROPERTY(BlueprintReadOnly, Category = "UI")
		TArray<class UGSAbilityTabWidget*> AbilitySets;

	FName DropSlotName;

protected:
	bool bIsWidgetCreated;
public:
	void InitializeWidget();

	UFUNCTION()
		void UpdateCreatetWidget();

	UFUNCTION()
		void UpdateWidget();
};
