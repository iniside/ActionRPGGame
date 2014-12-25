// Copyright 1998-2014 Epic Games, Inc. All Rights Reserved.
#pragma once
#include "FCTFloatingTextComponent.h"
#include "FCTGlobalTypes.h"
#include "GAGlobalTypes.h"
#include "GSPlayerController.generated.h"


UCLASS()
class GAMESYSTEM_API AGSPlayerController : public APlayerController
{
	GENERATED_UCLASS_BODY()

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "UI Cosmetics", meta = (AllowPrivateAccess = "true"))
	class UFCTFloatingTextComponent* FCTIndicator;

public:
	void BeginPlay() override;
};



