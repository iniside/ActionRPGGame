// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "ARWeaponListWidget.generated.h"

/**
 * 
 */
UCLASS()
class ACTIONRPGGAME_API UARWeaponListWidget : public UUserWidget
{
	GENERATED_BODY()
public:
	TWeakObjectPtr<class UARWeaponManagerComponent> WeaponManager;
	TWeakObjectPtr<class AARPlayerController> ARPC;
protected:
	UPROPERTY(BlueprintReadOnly, meta = (BindWidget))
		UWrapBox* ItemContainer;
	UPROPERTY(BlueprintReadOnly, meta = (BindWidget))
		class UARWeaponListSlotDropWidget* RightWeapon;
	UPROPERTY(BlueprintReadOnly, meta = (BindWidget))
		class UARWeaponListSlotDropWidget* LeftWeapon;
	UPROPERTY(BlueprintReadOnly, meta = (BindWidget))
		class UARWeaponListSlotDropWidget* BackDownWeapon;
	UPROPERTY(BlueprintReadOnly, meta = (BindWidget))
		class UARWeaponListSlotDropWidget* SideLeftWeapon;
protected:
	
	UPROPERTY()
		TArray<class UARWeaponListSlotDragWidget*> Items;
public:
	void NativePreConstruct();
	void NativeConstruct();
public:
	void AddItem(TSubclassOf<class UARWeaponListSlotDragWidget> DragWidgetClass, int32 WeaponIdx);
};
