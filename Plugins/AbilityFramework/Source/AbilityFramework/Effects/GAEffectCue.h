// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "GameFramework/Actor.h"
#include "MovieSceneSequencePlayer.h"
#include "GAGlobalTypes.h"
#include "GAEffectCue.generated.h"
class UActorSequencePlayer;
UCLASS()
class ABILITYFRAMEWORK_API AGAEffectCue : public AActor
{
	GENERATED_BODY()
public:	
	// Sets default values for this actor's properties
	AGAEffectCue(const FObjectInitializer& ObjectInitializer);
	virtual void PostInitProperties() override;
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
};
