// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "GameFramework/Actor.h"
#include "MovieSceneSequencePlayer.h"
#include "GAGlobalTypes.h"
#include "GameplayTags.h"
#include "AssetBundleData.h"
#include "Engine/AssetManager.h"
#include "AFCueActor.generated.h"
class UActorSequencePlayer;
UCLASS()
class ABILITYFRAMEWORK_API AAFCueActor : public AActor
{
	GENERATED_BODY()
protected:
	UPROPERTY(EditAnywhere, Category = "Cue")
		FGameplayTag CueTag;
	UPROPERTY(AssetRegistrySearchable)
		FName EffectCueTagSearch;
	UPROPERTY()
		FAssetBundleData AssetBundleData;
public:	
	// Sets default values for this actor's properties
	AAFCueActor(const FObjectInitializer& ObjectInitializer);
	void PostInitProperties() override;

	void Serialize(FArchive& Ar) override;

#if WITH_EDITORONLY_DATA
	void UpdateAssetBundleData();
	void PreSave(const class ITargetPlatform* TargetPlatform) override;
#endif //WITH_EDITORONLY_DATA
	void PostLoad() override;
	FPrimaryAssetId GetPrimaryAssetId() const override;
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;
	
	// Called every frame
	virtual void Tick( float DeltaSeconds ) override;


	UFUNCTION(BlueprintImplementableEvent)
		void BeginCue(AActor* InstigatorOut, AActor* TargetOut, UObject* Causer,
			const FHitResult& HitInfo);

	UFUNCTION(BlueprintImplementableEvent)
		void OnExecuted();

	UFUNCTION(BlueprintImplementableEvent)
		void OnExpired();

	UFUNCTION(BlueprintImplementableEvent)
		void OnRemoved();

	void NativeBeginCue(AActor* InstigatorOut, AActor* TargetOut, UObject* Causer,
		const FHitResult& HitInfo, const FGAEffectCueParams& CueParams);

	void NativeOnExecuted();
	void NativeOnRemoved();
	UPROPERTY()
		float Duration;
	UPROPERTY()
		float Period;

	void SetAnimation(class UGAEffectCueSequence* InSequence);
	UPROPERTY()
		double StartTime;
	UPROPERTY()
		double EndTime;
	/** Animation being played */
	UPROPERTY(EditAnywhere, Instanced, Export, Category = Animation)
		UGAEffectCueSequence* Sequence;
	UPROPERTY(transient, BlueprintReadOnly, Category = Animation)
		UActorSequencePlayer* SequencePlayer;

	UPROPERTY(EditAnywhere, Category = "Playback", meta = (ShowOnlyInnerProperties))
		FMovieSceneSequencePlaybackSettings PlaybackSettings;

	FTimerHandle PeriodTimer;

protected:
	void UpdateAssetRegistryInfo();
};
