// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "IFItemBase.h"
#include "ARItemBase.generated.h"

/**
 * 
 */
UCLASS()
class ACTIONRPGGAME_API UARItemBase : public UIFItemBase
{
	GENERATED_BODY()
public:
	//obviously we want TSoftObjectPtr<>
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Visual")
		UTexture2D* Icon;
	
	
};
