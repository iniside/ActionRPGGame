#pragma once
#include "UserWidget.h"
#include "../GSAbilityGlobalTypes.h"

#include "GSAbilityBookContainerWidget.generated.h"

/*
	It's not ability book, per se, but it contains non instanced abilities (classes).

	Use it as base to create real ability book, skill selection etc.
*/
UCLASS(BlueprintType, Blueprintable)
class GAMESYSTEM_API UGSAbilityBookContainerWidget : public UUserWidget
{
	GENERATED_UCLASS_BODY()
public:
	UPROPERTY(EditAnywhere, Category = "Ability Book UI")
		FGSAbilityBookUI AbilityBookConfig;

	UPROPERTY()
	class UGSAbilitiesComponent* OwningComp;

	UPROPERTY(BlueprintReadOnly, Category = "UI")
		TArray<class UGSAbilityBookTabWidget*> BookTabs;
protected:
	bool bWidgetCreated;
public:
	void InitializeWidget(const FGSAbilityBookUI& AbilityBookConfigIn,
	class UGSAbilitiesComponent* CompIn);
	void CreateUpdateWidget();
};
