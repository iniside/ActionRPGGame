// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameplayTags.h"
#include "AssetBundleData.h"
#include "UObject/NoExportTypes.h"
#include "GAGlobalTypes.h"
#include "AFCueStatic.generated.h"

/**
 * An Non instanced Cue, where only CDO is used to execute it.
 * They cannot have state or modify any external state.
 */
UCLASS(BlueprintType, Blueprintable, meta=(ShowWorldContextPin))
class ABILITYFRAMEWORK_API UAFCueStatic : public UObject
{
	GENERATED_BODY()
public:
	UPROPERTY(EditAnywhere, Category = "Cue")
		FGameplayTag CueTag;
	UPROPERTY(AssetRegistrySearchable)
		FName EffectCueTagSearch;
	UPROPERTY()
		FAssetBundleData AssetBundleData;
public:
	// Sets default values for this actor's properties
	UAFCueStatic(const FObjectInitializer& ObjectInitializer);
	void PostInitProperties() override;

	void Serialize(FArchive& Ar) override;

#if WITH_EDITORONLY_DATA
	void UpdateAssetBundleData();
	void PreSave(const class ITargetPlatform* TargetPlatform) override;
#endif //WITH_EDITORONLY_DATA
	void PostLoad() override;
	FPrimaryAssetId GetPrimaryAssetId() const override;
protected:
	void UpdateAssetRegistryInfo();

public:
	UFUNCTION(BlueprintNativeEvent, Category = "Ability Framework|Cues")
		 bool OnExecuted(const FGAEffectCueParams& Hit);
	virtual bool OnExecuted_Implementation(const FGAEffectCueParams& Hit) const;

	UFUNCTION(BlueprintNativeEvent, Category = "Ability Framework|Cues")
		void OnActivate(const FGAEffectCueParams& Hit) const;
	virtual void OnActivate_Implementation(const FGAEffectCueParams& Hit) const;

	UFUNCTION(BlueprintNativeEvent, Category = "Ability Framework|Cues")
		void OnExpire(const FGAEffectCueParams& Hit) const;
	virtual void OnExpire_Implementation(const FGAEffectCueParams& Hit) const;

	UFUNCTION(BlueprintNativeEvent, Category = "Ability Framework|Cues")
		void OnRemoved(const FGAEffectCueParams& Hit) const;
	virtual void OnRemoved_Implementation(const FGAEffectCueParams& Hit) const;

	UWorld* GetWorld() const override;
};
