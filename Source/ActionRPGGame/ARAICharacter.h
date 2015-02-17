// Copyright 1998-2014 Epic Games, Inc. All Rights Reserved.
#pragma once
#include "GameFramework/Character.h"

#include "GSCharacter.h"

#include "GAAttributeComponent.h"
#include "GISInventoryBaseComponent.h" //need for template

#include "Abilities/GSAbilitiesComponent.h"

#include "GAGlobalTypes.h"

#include "GISGlobalTypes.h"
#include "GISItemData.h"

#include "IGAAttributes.h"
#include "IGIPawn.h"
#include "Abilities/GSAbilitiesComponent.h"

#include "ARAICharacter.generated.h"

UCLASS(config=Game)
class AARAICharacter : public AGSCharacter, public IIGAAttributes,
	public IIGIPawn
{
	GENERATED_BODY()

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Attributes", meta = (AllowPrivateAccess = "true"))
	class UGAAttributeComponent* Attributes;

public:

	virtual float TakeDamage(float DamageAmount, struct FDamageEvent const& DamageEvent, class AController* EventInstigator, AActor* DamageCauser) override;
	/** IIGAAttributes Begin */
	UFUNCTION(BlueprintCallable, Category = "Game Attributes")
		virtual class UGAAttributesBase* GetAttributes() override;

	UFUNCTION(BlueprintCallable, Category = "Game Attributes")
		virtual class UGAAttributeComponent* GetAttributeComponent() override;

	UFUNCTION(BlueprintCallable, Category = "Game Attributes")
		virtual float GetAttributeValue(FGAAttribute AttributeIn) const;
	/** IIGAAttributes End */

	/** IIGIPawn */
	virtual APawn* GetGamePawn() override { return this; };
	virtual ACharacter* GetGameCharacter() override { return this; }
	virtual AController* GetGameController() override { return GetController(); }
	virtual APlayerController* GetGamePlayerController() override { return nullptr; };
	/* IIGIPawn **/

public:
	AARAICharacter(const FObjectInitializer& ObjectInitializer);
};

