// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "UI/HUD/AREnemyHealthBar.h"
#include "ARHUDWidget.h"
#include "SARDrawTestWidget.h"

#include "IFItemContainerWidget.h"

#include "ARUIComponent.generated.h"


UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class ACTIONRPGGAME_API UARUIComponent : public UActorComponent
{
	GENERATED_BODY()
protected:
	UPROPERTY(EditAnywhere, Category="Cross Hair")
		FSlateBrush Brush;

	UPROPERTY(EditAnywhere, Category = "Widgets")
		TSubclassOf<class UUserWidget> CrosshairClass;

	UPROPERTY()
		UUserWidget* CrosshairWidget;

	UPROPERTY(EditAnywhere, Category = "Widgets")
		TSubclassOf<class UARInventoryManagerWidget> InventoryManagerClass;
	UPROPERTY()
		class UARInventoryManagerWidget* InventoryManagerWidget;


	UPROPERTY(EditAnywhere, Category = "Widgets")
		TSubclassOf<class UAREnemyHealthBar> EnemyHealthBarClass;

	UPROPERTY(BlueprintReadOnly, Category = "Widgets")
		UAREnemyHealthBar* EnemyHealthBarWidget;

	UPROPERTY(EditAnywhere, Category = "Widgets")
		TSubclassOf<class UARHUDWidget> HUDWidgetClass;

	UPROPERTY(EditAnywhere, Category = "Widgets")
		TSubclassOf<class UIFItemContainerWidget> InventoryWidgetClass;

	UPROPERTY(BlueprintReadOnly, Category = "Widgets")
		UIFItemContainerWidget* InventoryWidget;


	UPROPERTY(EditAnywhere, Category = "Widgets")
		TSubclassOf<class UIFItemContainerWidget> WeaponInventoryWidgetClass;

	UPROPERTY(BlueprintReadOnly, Category = "Widgets")
		UIFItemContainerWidget* WeaponInventoryWidget;

public:
	UPROPERTY(BlueprintReadOnly, Category = "Widgets")
		UARHUDWidget* HUDWidget;


	TSharedPtr<SWidget> DrawWidget;
	TSharedPtr<SARDrawTestWidget> CrosshairWidget2;
public:	
	// Sets default values for this component's properties
	UARUIComponent();

protected:
	// Called when the game starts
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;

		
	void ShowHideInventory();
};
