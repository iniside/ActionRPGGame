// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "SARMainWidget.h"
#include "ARUIGlobals.h"
#include "SARFTContainer.h"
#include "SARTargetInfo.h"
#include "GAGlobalTypes.h"
#include "ARUIComponent.generated.h"

UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class ACTIONRPGGAME_API UARUIComponent : public UActorComponent
{
	GENERATED_BODY()
protected:
	//Main GameDesktop widget.
	//container for all other widgets.
	TSharedPtr<SARMainWidget> MainWidget;
	TSharedPtr<SARFTContainer> FloatingText;
	TSharedPtr<SARTargetInfo> TargetInfo;
	UPROPERTY()
		class AARCharacter* ARCharacter;
public:
	UPROPERTY(EditAnywhere, Category = "UI Config")
		FARFTConfig FloatingTextConfig;
	
	UPROPERTY(EditAnywhere, Category = "UI Config")
		FARTargetInfoConfig TargetInfoConfig;

public:	
	// Sets default values for this component's properties
	UARUIComponent();

protected:
	// Called when the game starts
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;
	virtual void EndPlay(const EEndPlayReason::Type EndPlayReason);
		
	UFUNCTION(BlueprintCallable, Category = "ActionRPGGame|Test")
		void EmitCombatText(FVector WorldPos, const FString& InText);

	UFUNCTION()
	void OnAttributeChanged(const FAFAttributeChangedData& Data);

	UFUNCTION(BlueprintCallable, Category = "ActionRPGGame|Test")
		void ShowTargetInfo(AActor* InTarget);
};
