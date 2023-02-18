// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "GameplayTags.h"
#include "AFAbilityComponent.h"
#include "AFEffectsComponent.h"

#include "AFAbilityInterface.h"

#include "ARAICharacter.generated.h"

UCLASS()
class ACTIONRPGGAME_API AARAICharacter : public ACharacter, public IAFAbilityInterface
{
	GENERATED_BODY()
		friend class AAREnemySpawner;
protected:
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Camera, meta = (AllowPrivateAccess = "true"))
		class UAFAbilityComponent* Abilities;
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Camera, meta = (AllowPrivateAccess = "true"))
		class UAFEffectsComponent* EffectsComponent;

	UPROPERTY(BlueprintReadOnly, Category = "ActionRPGGame|Spawn")
		class AAREnemySpawner* SpawnedBy;

public:
	// Sets default values for this character's properties
	AARAICharacter();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

	virtual void OnSpawned(class AAREnemySpawner* InSpawnedBy);

	
public:
	virtual void Kill();
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	// Called to bind functionality to input
	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;

	
	/* IAFAbilityInterface- BEGIN */
	UFUNCTION(BlueprintCallable, Category = "AbilityFramework|Attributes")
		virtual class UAFAbilityComponent* GetAbilityComp() override;

	UFUNCTION(BlueprintCallable, Category = "AbilityFramework|Attributes")
		virtual class UAFEffectsComponent* GetEffectsComponent() override;
	
	virtual class UAFEffectsComponent* NativeGetEffectsComponent() const override;

	UFUNCTION(BlueprintCallable, Category = "AbilityFramework|Attributes")
		virtual float GetAttributeValue(FGAAttribute AttributeIn) const override;

	virtual void ModifyAttribute(FGAEffectMod& ModIn, const FGAEffectHandle& HandleIn,
		struct FGAEffectProperty& InProperty, const FGAEffectContext& InContext) override;
	virtual FAFAttributeBase* GetAttribute(FGAAttribute AttributeIn) override;
	virtual void RemoveBonus(FGAAttribute AttributeIn, const FGAEffectHandle& HandleIn, EGAAttributeMod InMod) override;

	virtual float NativeGetAttributeValue(const FGAAttribute AttributeIn) const override;


	/* IAFAbilityInterface- END */
};
