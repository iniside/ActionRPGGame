// Copyright 1998-2014 Epic Games, Inc. All Rights Reserved.

#include "AbilityFrameworkEditor.h"
#include "Editor/PropertyEditor/Public/PropertyEditing.h"

#include "STextCombobox.h"
#include "STreeView.h"

#include "Effects/AFCueActor.h"
#include "Effects/GAEffectCueSequence.h"
#include "EditorReimportHandler.h"
#include "MovieScene.h"
#include "Tracks/MovieScenePropertyTrack.h"
#if WITH_EDITOR
#include "Editor.h"
#endif
#include "ISequencerModule.h"
#include "LevelEditorSequencerIntegration.h"
#include "SSCSEditor.h"
#include "SlateIconFinder.h"
#include "BlueprintEditorUtils.h"
#include "Framework/MultiBox/MultiBoxBuilder.h"
#include "Framework/Application/SlateApplication.h"

#include "AFEffectCueDetails.h"

class SEffectCueSequenceEditorWidget
	: public SCompoundWidget
{
private:
	TWeakObjectPtr<UGAEffectCueSequence> WeakSequence;
	AAFCueActor* EffectCue;
	TWeakPtr<FBlueprintEditor> WeakBlueprintEditor;

	TSharedPtr<SBox> Content;
	TSharedPtr<ISequencer> Sequencer;
	FDelegateHandle OnBlueprintPreCompileHandle;
	FDelegateHandle OnObjectSavedHandle;
	FDelegateHandle OnSequenceChangedHandle;
	FDelegateHandle OnBlueprintCompileHandle;
	FDelegateHandle OnBlueprintReinstancedHandle;
public:

	SLATE_BEGIN_ARGS(SEffectCueSequenceEditorWidget) {}
	SLATE_END_ARGS();

	void Construct(const FArguments&, TWeakPtr<FBlueprintEditor> InBlueprintEditor)
	{
		OnBlueprintPreCompileHandle = GEditor->OnBlueprintPreCompile().AddSP(this, &SEffectCueSequenceEditorWidget::OnBlueprintPreCompile);
		OnObjectSavedHandle = FCoreUObjectDelegates::OnObjectSaved.AddSP(this, &SEffectCueSequenceEditorWidget::OnObjectPreSave);
		OnBlueprintCompileHandle = GEditor->OnBlueprintCompiled().AddSP(this, &SEffectCueSequenceEditorWidget::OnBlueprintCompiled);
		OnBlueprintReinstancedHandle = GEditor->OnBlueprintReinstanced().AddSP(this, &SEffectCueSequenceEditorWidget::OnBlueprintReinstanced);

		WeakBlueprintEditor = InBlueprintEditor;

		ChildSlot
			[
				SAssignNew(Content, SBox)
				.MinDesiredHeight(200)
			];
	}
	void OnBlueprintCompiled()
	{
		TSharedPtr<FBlueprintEditor> BlueprintEditor = WeakBlueprintEditor.Pin();
		UBlueprint* bp = BlueprintEditor->GetBlueprintObj();
		float dupa = 0;
	}
	void OnBlueprintReinstanced()
	{
		float dupa = 0;
	}
	~SEffectCueSequenceEditorWidget()
	{
		if (Sequencer.IsValid())
		{
			Sequencer->Close();
			Sequencer = nullptr;
		}

		GEditor->OnBlueprintPreCompile().Remove(OnBlueprintPreCompileHandle);
		GEditor->OnBlueprintCompiled().Remove(OnBlueprintCompileHandle);
		GEditor->OnBlueprintReinstanced().Remove(OnBlueprintReinstancedHandle);
		FCoreUObjectDelegates::OnObjectSaved.Remove(OnObjectSavedHandle);
	}
	void OnObjectPreSave(UObject* InObject)
	{
		TSharedPtr<FBlueprintEditor> BlueprintEditor = WeakBlueprintEditor.Pin();
		if (Sequencer.IsValid() && BlueprintEditor.IsValid() && InObject && InObject == BlueprintEditor->GetBlueprintObj())
		{
			Sequencer->RestorePreAnimatedState();
		}
	}

	void OnBlueprintPreCompile(UBlueprint* InBlueprint)
	{
		TSharedPtr<FBlueprintEditor> BlueprintEditor = WeakBlueprintEditor.Pin();
		if (Sequencer.IsValid() && BlueprintEditor.IsValid() && InBlueprint && InBlueprint == BlueprintEditor->GetBlueprintObj())
		{
			Sequencer->RestorePreAnimatedState();
		}
	}
	void OnSequenceChanged()
	{
		UGAEffectCueSequence* ActorSequence = WeakSequence.Get();
		/*UBlueprint* Blueprint = ActorSequence ? ActorSequence->GetParentBlueprint() : nullptr;

		if (Blueprint)
		{
			FBlueprintEditorUtils::MarkBlueprintAsModified(Blueprint);
		}*/
	}
	void OnSequencerReceivedFocus()
	{
		if (Sequencer.IsValid())
		{
			FLevelEditorSequencerIntegration::Get().OnSequencerReceivedFocus(Sequencer.ToSharedRef());
		}
	}
	UObject* GetPlaybackContext() const
	{
		UGAEffectCueSequence* LocalActorSequence = WeakSequence.Get();
		if (LocalActorSequence)
		{
			if (AActor* Actor = LocalActorSequence->GetTypedOuter<AActor>())
			{
				return Actor;
			}
			else if (UBlueprintGeneratedClass* GeneratedClass = LocalActorSequence->GetTypedOuter<UBlueprintGeneratedClass>())
			{
				return GeneratedClass->SimpleConstructionScript->GetComponentEditorActorInstance();
			}
		}

		return nullptr;
	}

	TArray<UObject*> GetEventContexts() const
	{
		TArray<UObject*> Contexts;
		if (auto* Context = GetPlaybackContext())
		{
			Contexts.Add(Context);
		}
		return Contexts;
	}
	AActor* GetPreviewActor() const
	{
		TSharedPtr<FBlueprintEditor> BlueprintEditor = WeakBlueprintEditor.Pin();
		if (BlueprintEditor.IsValid())
		{
			return BlueprintEditor->GetPreviewActor();
		}
		if (UGAEffectCueSequence* Sequence = WeakSequence.Get())
		{
			return Sequence->GetTypedOuter<AActor>();
		}
		return nullptr;
	}
	void OnSelectionUpdated(TSharedPtr<FSCSEditorTreeNode> SelectedNode)
	{
		if (SelectedNode->GetNodeType() != FSCSEditorTreeNode::ComponentNode)
		{
			return;
		}

		UActorComponent* EditingComponent = nullptr;

		TSharedPtr<FBlueprintEditor> BlueprintEditor = WeakBlueprintEditor.Pin();
		if (BlueprintEditor.IsValid())
		{
			UBlueprint* Blueprint = BlueprintEditor->GetBlueprintObj();
			if (Blueprint)
			{
				EditingComponent = SelectedNode->GetEditableComponentTemplate(Blueprint);
			}
		}
		else if (AActor* Actor = GetPreviewActor())
		{
			EditingComponent = SelectedNode->FindComponentInstanceInActor(Actor);
		}

		if (EditingComponent)
		{
			const FScopedTransaction Transaction(FText::FromString("Add component to Sequencer"));
			Sequencer->GetHandleToObject(EditingComponent, true);
		}

		FSlateApplication::Get().DismissAllMenus();
	}
	void AddPossessComponentMenuExtensions(FMenuBuilder& MenuBuilder)
	{
		AActor* Actor = GetPreviewActor();
		if (!Actor)
		{
			return;
		}

		Sequencer->State.ClearObjectCaches(*Sequencer);
		TSet<UObject*> AllBoundObjects;
		if (UGAEffectCueSequence* Sequence = WeakSequence.Get())
		{
			AllBoundObjects.Add(Sequence->GetTypedOuter<AActor>());
			//return Sequence->GetTypedOuter<AActor>();
		}
		UMovieScene* MovieScene = Sequencer->GetFocusedMovieSceneSequence()->GetMovieScene();
		for (int32 Index = 0; Index < MovieScene->GetPossessableCount(); ++Index)
		{
			FMovieScenePossessable& Possessable = MovieScene->GetPossessable(Index);
			for (TWeakObjectPtr<> WeakObject : Sequencer->FindBoundObjects(Possessable.GetGuid(), Sequencer->GetFocusedTemplateID()))
			{
				if (UObject* Object = WeakObject.Get())
				{
					AllBoundObjects.Add(Object);
				}
			}
		}

		bool bIdent = false;
		/*MenuBuilder.AddWidget(
			SNew(SComponentSelectionTree, Actor)
			.IsInEditMode(WeakBlueprintEditor.Pin().IsValid())
			.OnComponentSelected(this, &SEffectCueSequenceEditorWidget::OnSelectionUpdated)
			.IsComponentValid_Lambda(
				[AllBoundObjects](UActorComponent* Component)
				{
					return !AllBoundObjects.Contains(Component);
				}
			)
			, FText(), !bIdent
			);*/
	}
	void SetActorSequence(UGAEffectCueSequence* NewSequence, AAFCueActor* InEffectCue)
	{
		if (UGAEffectCueSequence* OldSequence = WeakSequence.Get())
		{
			if (OnSequenceChangedHandle.IsValid())
			{
				OldSequence->OnSignatureChanged().Remove(OnSequenceChangedHandle);
			}
		}
		EffectCue = InEffectCue;
		WeakSequence = NewSequence;

		if (NewSequence)
		{
			OnSequenceChangedHandle = NewSequence->OnSignatureChanged().AddSP(this, &SEffectCueSequenceEditorWidget::OnSequenceChanged);
		}

		// If we already have a sequencer open, just assign the sequence
		if (Sequencer.IsValid() && NewSequence)
		{
			if (Sequencer->GetRootMovieSceneSequence() != NewSequence)
			{
				Sequencer->ResetToNewRootSequence(*NewSequence);
			}
			return;
		}

		// If we're setting the sequence to none, destroy sequencer
		if (!NewSequence)
		{
			//Content->SetContent(SNew(STextBlock).Text(LOCTEXT("NothingSelected", "Select a sequence")));
			return;
		}

		// We need to initialize a new sequencer instance
		FSequencerInitParams SequencerInitParams;
		{
			TWeakObjectPtr<UGAEffectCueSequence> LocalWeakSequence = NewSequence;

			SequencerInitParams.RootSequence = NewSequence;
			SequencerInitParams.EventContexts = TAttribute<TArray<UObject*>>(this, &SEffectCueSequenceEditorWidget::GetEventContexts);
			SequencerInitParams.PlaybackContext = TAttribute<UObject*>(this, &SEffectCueSequenceEditorWidget::GetPlaybackContext);

			TSharedRef<FExtender> AddMenuExtender = MakeShareable(new FExtender);

			AddMenuExtender->AddMenuExtension("AddTracks", EExtensionHook::Before, nullptr,
				FMenuExtensionDelegate::CreateLambda([=](FMenuBuilder& MenuBuilder) {

				MenuBuilder.AddSubMenu(
					FText::FromString("Component"),
					FText::FromString("Add a binding to one of this actor's components and allow it to be animated by Sequencer"),
					FNewMenuDelegate::CreateRaw(this, &SEffectCueSequenceEditorWidget::AddPossessComponentMenuExtensions),
					false /*bInOpenSubMenuOnClick*/,
					FSlateIcon()//"LevelSequenceEditorStyle", "LevelSequenceEditor.PossessNewActor")
				);

			})
			);

			SequencerInitParams.ViewParams.bReadOnly = false;// !WeakBlueprintEditor.IsValid() && !NewSequence->IsEditable();
			SequencerInitParams.bEditWithinLevelEditor = false;
			SequencerInitParams.ViewParams.AddMenuExtender = AddMenuExtender;
			SequencerInitParams.ViewParams.UniqueName = "EffectCueActorSequenceEditor";
			SequencerInitParams.ViewParams.OnReceivedFocus.BindRaw(this, &SEffectCueSequenceEditorWidget::OnSequencerReceivedFocus);
		}

		Sequencer = FModuleManager::LoadModuleChecked<ISequencerModule>("Sequencer").CreateSequencer(SequencerInitParams);
		Content->SetContent(Sequencer->GetSequencerWidget());
	}
};



FEffectCueSequenceEditorSummoner::FEffectCueSequenceEditorSummoner(TSharedPtr<FBlueprintEditor> BlueprintEditor)
	: FWorkflowTabFactory("EmbeddedEffectCueSequenceID", BlueprintEditor)
	, WeakBlueprintEditor(BlueprintEditor)
{
	bIsSingleton = true;

	TabLabel = FText::FromString("Cue Sequencer");
}

TSharedRef<SWidget> FEffectCueSequenceEditorSummoner::CreateTabBody(const FWorkflowTabSpawnInfo& Info) const
{
	return SNew(SEffectCueSequenceEditorWidget, WeakBlueprintEditor);
}

TSharedRef<IDetailCustomization> FAFEffectCueDetails::MakeInstance()
{
	return MakeShareable(new FAFEffectCueDetails);
}

void FAFEffectCueDetails::CustomizeDetails(IDetailLayoutBuilder& DetailLayout)
{
	const IDetailsView* DetailsView = DetailLayout.GetDetailsView();
	if (DetailsView)
	{
		TSharedPtr<FTabManager> HostTabManager = DetailsView->GetHostTabManager();
		TArray<TWeakObjectPtr<UObject>> Objects;
		DetailLayout.GetObjectsBeingCustomized(Objects);
		AAFCueActor* EffectCue = Cast<AAFCueActor>(Objects[0].Get());

		if (HostTabManager.IsValid() && HostTabManager->CanSpawnTab("EmbeddedEffectCueSequenceID"))
		{
			TSharedPtr<SDockTab> ExistingTab = HostTabManager->FindExistingLiveTab(FName("EmbeddedEffectCueSequenceID"));
			if (ExistingTab.IsValid())
			{
				//EffectCue->StaticClass()->GetDefaultObject<AAFCueActor>()->Sequence

				//auto SequencerWidget = StaticCastSharedRef<SEffectCueSequenceEditorWidget>(ExistingTab->GetContent());
				StaticCastSharedRef<SEffectCueSequenceEditorWidget>(ExistingTab->GetContent())->SetActorSequence(EffectCue->StaticClass()->GetDefaultObject<AAFCueActor>()->Sequence, EffectCue);
				//bIsExternalTabAlreadyOpened = ThisSequence && SequencerWidget->GetSequence() == ThisSequence;
				return;
			}
			//EffectCue->Sequence
			if (!Tab.IsValid())
				Tab = HostTabManager->InvokeTab(FName("EmbeddedEffectCueSequenceID"));
			//Tab->SetContent(Sequencer->GetSequencerWidget());
			StaticCastSharedRef<SEffectCueSequenceEditorWidget>(Tab->GetContent())->SetActorSequence(EffectCue->StaticClass()->GetDefaultObject<AAFCueActor>()->Sequence, EffectCue);
		}
	}
}