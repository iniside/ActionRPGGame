// Copyright 1998-2014 Epic Games, Inc. All Rights Reserved.
#pragma once
#include "GameFramework/Character.h"

#include "GSCharacter.h"

#include "GAAbilitiesComponent.h"
#include "GISInventoryBaseComponent.h" //need for template

#include "Abilities/GSAbilitiesComponent.h"

#include "GAGlobalTypes.h"

#include "GISGlobalTypes.h"
#include "GISItemData.h"

#include "IGAAbilities.h"
#include "IGIPawn.h"
#include "Abilities/GSAbilitiesComponent.h"

#include "ARAICharacter.generated.h"

UCLASS(config=Game)
class AARAICharacter : public AGSCharacter, public IIGAAbilities,
	public IIGIPawn
{
	GENERATED_BODY()

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Attributes", meta = (AllowPrivateAccess = "true"))
	class UGAAbilitiesComponent* Attributes;

public:

	virtual float TakeDamage(float DamageAmount, struct FDamageEvent const& DamageEvent, class AController* EventInstigator, AActor* DamageCauser) override;
	/** IIGAAbilities Begin */
	UFUNCTION(BlueprintCallable, Category = "Game Attributes")
		virtual class UGAAttributesBase* GetAttributes() override;

	UFUNCTION(BlueprintCallable, Category = "Game Attributes")
		virtual class UGAAbilitiesComponent* GetAbilityComp() override;

	UFUNCTION(BlueprintCallable, Category = "Game Attributes")
		virtual float GetAttributeValue(FGAAttribute AttributeIn) const;
	/** IIGAAbilities End */

	/** IIGIPawn */
	virtual APawn* GetGamePawn() override { return this; };
	virtual ACharacter* GetGameCharacter() override { return this; }
	virtual AController* GetGameController() override { return GetController(); }
	virtual APlayerController* GetGamePlayerController() override { return nullptr; };
	/* IIGIPawn **/

public:
	AARAICharacter(const FObjectInitializer& ObjectInitializer);
};

