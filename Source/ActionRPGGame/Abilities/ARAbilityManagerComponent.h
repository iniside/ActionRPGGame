// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "GameplayTags.h"

#include "Abilities/GAAbilityBase.h"
#include "AMAbilityManagerComponent.h"
#include "DWTypes.h"


#include "ARAbilityManagerComponent.generated.h"


UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class ACTIONRPGGAME_API UARAbilityManagerComponent : public UAMAbilityManagerComponent
{
	GENERATED_BODY()
protected:
	UPROPERTY(EditAnywhere, Category = "Widget Config")
		TSubclassOf<class UARAbilityDragVisual> DragVisualClass;

	UPROPERTY(EditAnywhere, Category = "Widget Config")
		TSubclassOf<class UARAbilityManagerWidget> ManagerWidgetClass;


	UPROPERTY()
		class UARAbilityManagerWidget* ManagerWidget;

	FDWWWindowHandle ManagerWindowHandle;
public:	
	// Sets default values for this component's properties
	UARAbilityManagerComponent();

protected:
	// Called when the game starts
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;

	void ShowHideAbilityManager();
	
	inline TSubclassOf<class UARAbilityDragVisual> GetDragVisualClass()
	{
		return DragVisualClass;
	}
};
