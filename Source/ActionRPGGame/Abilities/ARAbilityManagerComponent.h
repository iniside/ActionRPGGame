// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "GameplayTags.h"

#include "Abilities/GAAbilityBase.h"
#include "AMAbilityManagerComponent.h"
#include "DWTypes.h"


#include "ARAbilityManagerComponent.generated.h"

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FAROnAbilitySetChanged, EAMGroup, Group);

USTRUCT(BlueprintType)
struct FARAbilityItem
{
	GENERATED_BODY()
public:
	UPROPERTY(EditAnywhere)
		FGameplayTag Ability;
};
UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class ACTIONRPGGAME_API UARAbilityManagerComponent : public UAMAbilityManagerComponent
{
	GENERATED_BODY()
protected:
	UPROPERTY(EditAnywhere, Category = "Widget Config")
		TSubclassOf<class UARAbilityDragVisual> DragVisualClass;

	UPROPERTY(EditAnywhere, Category = "Widget Config")
		TSubclassOf<class UARAbilityListWidget> ManagerWidgetClass;

	UPROPERTY(EditAnywhere, Category = "Widget Config")
		TSubclassOf<class UARAbilityListSlotDragWidget> DragWidgetClass;

	UPROPERTY()
		class UARAbilityListWidget* ManagerWidget;

	UPROPERTY(EditAnywhere)
		TArray<FARAbilityItem> AvailableAbilities;

	FDWWWindowHandle ManagerWindowHandle;
public:
	UPROPERTY(BlueprintAssignable)
		FAROnAbilitySetChanged OnAbilitySetChanged;
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

	UFUNCTION(BlueprintCallable, Category = "ActionRPGGame|Ability Manager")
		void SetCurrentSet(int32 SetIndex);

	virtual void OnGroupSelectionConfirmed(EAMGroup ValidGroup, bool bPredictionSuccess) override;

	void OnInputReady(FGameplayTag WeaponAbilityTag, TArray<FGameplayTag> InInputTags);
};
