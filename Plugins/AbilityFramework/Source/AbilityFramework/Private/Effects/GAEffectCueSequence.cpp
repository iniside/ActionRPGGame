// Fill out your copyright notice in the Description page of Project Settings.

#include "../AbilityFramework.h"
#include "AFCueActor.h"
#include "MovieScene.h"
#include "MovieSceneCommonHelpers.h"
#include "Modules/ModuleManager.h"
#include "Engine/BlueprintGeneratedClass.h"
#include "Engine/Blueprint.h"
#include "GameFramework/Actor.h"
#include "Classes/Engine/SCS_Node.h"
#include "Classes/Engine/SimpleConstructionScript.h"
#include "Engine/Blueprint.h"
#include "UObject/Package.h"
#include "ActorSequenceObjectReference.h"
#include "GAEffectCueSequence.h"


#if WITH_EDITOR
UGAEffectCueSequence::FOnInitialize UGAEffectCueSequence::OnInitializeSequenceEvent;
#endif

UGAEffectCueSequence::UGAEffectCueSequence(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
	, MovieScene(nullptr)
#if WITH_EDITOR
	, bHasBeenInitialized(false)
#endif
{
	bParentContextsAreSignificant = true;

	MovieScene = ObjectInitializer.CreateDefaultSubobject<UMovieScene>(this, "MovieScene");
	MovieScene->SetFlags(RF_Transactional);
}

void UGAEffectCueSequence::PostInitProperties()
{
#if WITH_EDITOR && WITH_EDITORONLY_DATA
	AAFCueActor* OwnerCue = Cast<AAFCueActor>(GetOuter());
	if (!bHasBeenInitialized && !HasAnyFlags(RF_ClassDefaultObject) && OwnerCue && !OwnerCue->HasAnyFlags(RF_ClassDefaultObject))
	{
		FGuid BindingID = MovieScene->AddPossessable(OwnerCue ? OwnerCue->GetActorLabel() : TEXT("Owner"), OwnerCue ? OwnerCue->GetClass() : AAFCueActor::StaticClass());
		//ObjectReferences.CreateBinding(BindingID, FActorSequenceObjectReference::CreateForContextActor());

		OnInitializeSequenceEvent.Broadcast(this);
		bHasBeenInitialized = true;
	}
#endif

	Super::PostInitProperties();
}
#if WITH_EDITOR
UGAEffectCueSequence* UGAEffectCueSequence::GetNullAnimation()
{
	static UGAEffectCueSequence* NullAnimation = nullptr;

	if (!NullAnimation)
	{
		NullAnimation = NewObject<UGAEffectCueSequence>(GetTransientPackage(), NAME_None);
		NullAnimation->AddToRoot();
		NullAnimation->MovieScene = NewObject<UMovieScene>(NullAnimation, FName("No Animation"));
		NullAnimation->MovieScene->AddToRoot();
	}

	return NullAnimation;
}
#endif //WITH_EDITOR
FFrameNumber UGAEffectCueSequence::GetStartTime() const
{
	return MovieScene->GetPlaybackRange().GetLowerBoundValue();
}


FFrameNumber UGAEffectCueSequence::GetEndTime() const
{
	return MovieScene->GetPlaybackRange().GetUpperBoundValue();
}


/* UMovieSceneAnimation overrides
*****************************************************************************/


void UGAEffectCueSequence::BindPossessableObject(const FGuid& ObjectId, UObject& PossessedObject, UObject* Context)
{
	AActor* ActorContext = CastChecked<AActor>(Context);

	if (AActor* Actor = Cast<AActor>(&PossessedObject))
	{
		//ObjectReferences.CreateBinding(ObjectId, FActorSequenceObjectReference::CreateForActor(Actor, ActorContext));
	}
}


bool UGAEffectCueSequence::CanPossessObject(UObject& Object, UObject* InPlaybackContext) const
{
	AActor* ActorContext = CastChecked<AActor>(InPlaybackContext);

	if (AActor* Actor = Cast<AActor>(&Object))
	{
		return Actor == InPlaybackContext || Actor->GetLevel() == ActorContext->GetLevel();
	}
	return true;
}

void UGAEffectCueSequence::LocateBoundObjects(const FGuid& ObjectId, UObject* Context, TArray<UObject*, TInlineAllocator<1>>& OutObjects) const
{
	if (Context)
	{
		//ObjectReferences.ResolveBinding(ObjectId, CastChecked<AActor>(Context), OutObjects);
	}
}


UMovieScene* UGAEffectCueSequence::GetMovieScene() const
{
	return MovieScene;
}


UObject* UGAEffectCueSequence::GetParentObject(UObject* Object) const
{

	return GetOuter();
}

void UGAEffectCueSequence::UnbindPossessableObjects(const FGuid& ObjectId)
{
	//ObjectReferences.RemoveBinding(ObjectId);
}
