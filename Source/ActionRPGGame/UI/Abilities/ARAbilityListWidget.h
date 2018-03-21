// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "Components/WrapBox.h"
#include "ARAbilityListWidget.generated.h"

/**
 * 
 */
UCLASS()
class ACTIONRPGGAME_API UARAbilityListWidget : public UUserWidget
{
	GENERATED_BODY()
public:
	TWeakObjectPtr<class AARPlayerController> ARPC;
protected:
	UPROPERTY(BlueprintReadOnly, meta = (BindWidget))
		UWrapBox* ItemContainer;

	//Currently Equiped abilities
	UPROPERTY(BlueprintReadOnly, meta = (BindWidget))
		class UARAbilityListSlotDropWidget* Ability001Group001;
	UPROPERTY(BlueprintReadOnly, meta = (BindWidget))
		class UARAbilityListSlotDropWidget* Ability002Group001;
	UPROPERTY(BlueprintReadOnly, meta = (BindWidget))
		class UARAbilityListSlotDropWidget* Ability003Group001;

	UPROPERTY()
		TArray<class UARAbilityListSlotDragWidget*> Items;
public:
	void AddItem(TSubclassOf<class UARAbilityListSlotDragWidget> DragWidgetClass, const FGameplayTag& Ability);
};
