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

	virtual void ModifyAttribute(FGAEffectMod& ModIn, const FGAEffectHandle& HandleIn, FGAEffectProperty& InProperty);// override { DefaultAttributes->ModifyAttribute(ModIn, HandleIn); };
	virtual FAFAttributeBase* GetAttribute(FGAAttribute AttributeIn);// override { return DefaultAttributes->GetAttribute(AttributeIn); };
	virtual void RemoveBonus(FGAAttribute AttributeIn, const FGAEffectHandle& HandleIn, EGAAttributeMod InMod);// override { DefaultAttributes->RemoveBonus(AttributeIn, HandleIn); };
	virtual FGAEffectHandle ApplyEffectToTarget(FGAEffect* EffectIn,
		FGAEffectProperty& InProperty, FGAEffectContext& InContext) override;// { return ApplyEffectToTarget(EffectIn, HandleIn); };
	virtual void RemoveTagContainer(const FGameplayTagContainer& TagsIn) override;
	virtual float NativeGetAttributeValue(const FGAAttribute AttributeIn) const;// override { return 0; };
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

