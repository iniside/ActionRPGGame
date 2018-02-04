// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "ARUIComponent.generated.h"


UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class ACTIONRPGGAME_API UARUIComponent : public UActorComponent
{
	GENERATED_BODY()
protected:
	UPROPERTY(EditAnywhere, Category = "Widgets")
		TSubclassOf<class UUserWidget> CrosshairClass;

	UPROPERTY()
		UUserWidget* CrosshairWidget;

	UPROPERTY(EditAnywhere, Category = "Widgets")
		TSubclassOf<class UARInventoryManagerWidget> InventoryManagerClass;
	UPROPERTY()
		class UARInventoryManagerWidget* InventoryManagerWidget;

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
