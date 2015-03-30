#pragma once
#include "UserWidget.h"
#include "GSAbilityBookItem.generated.h"

/*
	It's not ability book, per se, but it contains non instanced abilities (classes).

	Use it as base to create real ability book, skill selection etc.
*/
UCLASS(BlueprintType, Blueprintable)
class GAMESYSTEM_API UGSAbilityBookItem : public UUserWidget
{
	GENERATED_UCLASS_BODY()
public:
	UPROPERTY()
		FGSAbilityBookSlot SlotInfo;
public:
	UFUNCTION(BlueprintPure, Category = "UI")
		UTexture2D* GetAbilityIcon() const;
};
