#pragma once
#include "UserWidget.h"
#include "GSAbilityWidget.generated.h"

UCLASS(BlueprintType, Blueprintable)
class GAMESYSTEM_API UGSAbilityWidget : public UUserWidget
{
	GENERATED_UCLASS_BODY()
public:
	int32 AbilityIndex;

	UPROPERTY()
	class UGSAbilitiesComponent* AbilityComponent;
protected:
	UPROPERTY()
	class UGSAbility* AbilityPtrCache;
public:
	void UpdateWidgetData();
protected:
	UFUNCTION(BlueprintPure, Category = "UI")
		float GetAbilityCooldown() const;

	UFUNCTION(BlueprintPure, Category = "UI")
		UTexture2D* GetAbilityIcon() const;
};
