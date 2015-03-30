// Copyright 1998-2014 Epic Games, Inc. All Rights Reserved.
#pragma once
#include "FCTFloatingTextComponent.h"
#include "GSPlayerController.h"
#include "FCTGlobalTypes.h"
#include "GAGlobalTypes.h"
#include "ARPlayerController.generated.h"


UCLASS()
class ACTIONRPGGAME_API AARPlayerController : public AGSPlayerController
{
	GENERATED_BODY()
protected:
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Inventory", meta = (AllowPrivateAccess = "true"))
	class UGISInventoryBaseComponent* Inventory;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Abilities")
	class UGSAbilitiesComponent* Abilities;
public:
	AARPlayerController(const FObjectInitializer& ObjectInitializer);
	/** AActor Overrides - BEGIN */
	virtual void BeginPlay() override;
	/* AActor Overrides - END **/
	
	virtual void OnRep_Pawn() override;
	virtual void SetupInputComponent() override;


	UFUNCTION()
		void OnRecivedModifiedAttribute(const FGAModifiedAttribute& AttributeModIn);

	UFUNCTION(BlueprintImplementableEvent)
		void OnPawnReplicated(APawn* NewPawn);

	//virtual void PreInitializeComponents() override;
};



