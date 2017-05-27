// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "GameFramework/Actor.h"
#include "MovieSceneSequencePlayer.h"
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
		void BeginCue();
	void NativeBeginCue();


	//DECLARE_EVENT_OneParam(UUMGSequencePlayer, FOnSequenceFinishedPlaying, UUMGSequencePlayer&);
	//FOnSequenceFinishedPlaying& OnSequenceFinishedPlaying() { return OnSequenceFinishedPlayingEvent; }
	
	void SetAnimation(class UGAEffectCueSequence* InSequence);
	UPROPERTY()
		double StartTime;
	UPROPERTY()
		double EndTime;
	/** Animation being played */
	UPROPERTY(Instanced)
		class UGAEffectCueSequence* Sequence;
	UPROPERTY(Instanced)
		UActorSequencePlayer* SequencePlayer;
	UPROPERTY(EditAnywhere, Category = "Playback", meta = (ShowOnlyInnerProperties))
		FMovieSceneSequencePlaybackSettings PlaybackSettings;
};
