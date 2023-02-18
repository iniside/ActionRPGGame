// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "GameFramework/Actor.h"
#include "MovieSceneSequence.h"
#include "MovieScene.h"
#include "LazyObjectPtr.h"
#include "ActorSequence.h"
#include "FrameNumber.h"
#include "GAEffectCueSequence.generated.h"

UCLASS(BlueprintType, DefaultToInstanced)
class ABILITYFRAMEWORK_API UGAEffectCueSequence : public UMovieSceneSequence
{
	GENERATED_BODY()
	
public:	
	UPROPERTY(Instanced)
		UMovieScene* MovieScene;
private:
	/** Collection of object references. */
	//UPROPERTY()
	//	FActorSequenceObjectReferenceMap ObjectReferences;

#if WITH_EDITORONLY_DATA
private:
		bool bHasBeenInitialized;
#endif
public:
	// Sets default values for this actor's properties
	UGAEffectCueSequence(const FObjectInitializer& ObjectInitializer);
	virtual void PostInitProperties() override;
#if WITH_EDITOR
	/**
	* Get a placeholder animation.
	*
	* @return Placeholder animation.
	*/
	static UGAEffectCueSequence* GetNullAnimation();

	/** Event that is fired to initialize default state for a sequence */
	DECLARE_EVENT_OneParam(UGAEffectCueSequence, FOnInitialize, UGAEffectCueSequence*)
	static FOnInitialize OnInitializeSequenceEvent;
public:
	static FOnInitialize& OnInitializeSequence() { return OnInitializeSequenceEvent; }


#endif
public:
	inline UMovieScene* GetMovieScene() { return MovieScene; }
	UFUNCTION(BlueprintCallable, Category = "Animation")
		FFrameNumber GetStartTime() const;

	/**
	* Get the end time of this animation.
	*
	* @return End time in seconds.
	* @see GetStartTime
	*/
	UFUNCTION(BlueprintCallable, Category = "Animation")
		FFrameNumber GetEndTime() const;
	virtual void BindPossessableObject(const FGuid& ObjectId, UObject& PossessedObject, UObject* Context) override;
	virtual bool CanPossessObject(UObject& Object, UObject* InPlaybackContext) const override;
	virtual UMovieScene* GetMovieScene() const override;
	virtual UObject* GetParentObject(UObject* Object) const override;
	virtual void UnbindPossessableObjects(const FGuid& ObjectId) override;
	virtual void LocateBoundObjects(const FGuid& ObjectId, UObject* Context, TArray<UObject*, TInlineAllocator<1>>& OutObjects) const override;
};
