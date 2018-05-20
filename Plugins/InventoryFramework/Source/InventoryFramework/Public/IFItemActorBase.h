// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "IFItemActorBase.generated.h"

UCLASS()
class INVENTORYFRAMEWORK_API AIFItemActorBase : public AActor
{
	GENERATED_BODY()
		friend class UIFInventoryComponent;
protected:
	//temp
	UPROPERTY(EditAnywhere, Category = "Loot")
		TArray<TSoftClassPtr<class UIFItemBase>> Items;
public:	
	// Sets default values for this actor's properties
	AIFItemActorBase();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	virtual void OnItemPicked() {};
protected:

	TArray<TSoftClassPtr<class UIFItemBase>> GetAllItems();
	
	void GiveAllItemsToInstigator(class UIFInventoryComponent* Inventory);
	UFUNCTION(BlueprintCallable, Category = "InventoryFramework")
		void BP_GiveAllItemsToInstigator(class UIFInventoryComponent* Inventory);
};
