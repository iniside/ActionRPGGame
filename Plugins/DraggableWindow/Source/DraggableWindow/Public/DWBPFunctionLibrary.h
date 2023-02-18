// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Kismet/BlueprintFunctionLibrary.h"
#include "UserWidget.h"
#include "DWTypes.h"

#include "DWBPFunctionLibrary.generated.h"

/**
 * 
 */
UCLASS()
class DRAGGABLEWINDOW_API UDWBPFunctionLibrary : public UBlueprintFunctionLibrary
{
	GENERATED_BODY()
	
public:
	UFUNCTION(BlueprintCallable, Category = "Draggable Window")
		static FDWWWindowHandle CreateWindow(const FString& WindowName);
	
	UFUNCTION(BlueprintCallable, Category = "Draggable Window")
		static FDWWWindowHandle CreateWindowWithContent(UUserWidget* InWindowContent, const FString& WindowName);
};
