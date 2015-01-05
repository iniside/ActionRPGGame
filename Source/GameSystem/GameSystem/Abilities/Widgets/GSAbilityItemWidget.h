#pragma once
#include "UserWidget.h"
#include "Widgets/GISItemBaseWidget.h"
#include "GSAbilityItemWidget.generated.h"
/*
	Base class to show abilities in UI.
*/
UCLASS(BlueprintType, Blueprintable, DefaultToInstanced)
class GAMESYSTEM_API UGSAbilityItemWidget : public UGISItemBaseWidget
{
	GENERATED_UCLASS_BODY()
public:
	UPROPERTY()
	class UGSAbilitiesComponent* AbilityComponent;

	UPROPERTY()
	class UGSAbilityInfo* AbilityInfo;

	virtual void InitializeItem() override;

	UFUNCTION(BlueprintCallable, Category = "Ability Info")
		float const GetCurrentCooldownTime() const;

	UFUNCTION(BlueprintCallable, Category = "Ability Info")
		float const GetMaxCooldownTime() const;
};
