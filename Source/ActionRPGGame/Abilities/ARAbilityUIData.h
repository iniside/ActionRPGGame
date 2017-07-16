// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "UObject/NoExportTypes.h"
#include "ARAbilityUIData.generated.h"

/**
 * 
 */
UCLASS(Blueprintable, BlueprintType, EditInLineNew)
class ACTIONRPGGAME_API UARAbilityUIData : public UObject
{
	GENERATED_BODY()
public:
	UPROPERTY(EditAnywhere, BlueprintReadOnly)
		FText DisplayName;
	UPROPERTY(EditAnywhere, BlueprintReadOnly)
		FText Description;

	UPROPERTY(EditAnywhere, BlueprintReadOnly)
		UTexture2D* Icon;

};
