#include "WALandscapeGraphAssetEditor.h"
#include "WALandscapeGraphAssetEditorToolbar.h"
#include "WALandscapeGraphSchema.h"
#include "WALandscapeGraphEditorCommands.h"
#include "WALandscapeGraphEdGraph.h"
#include "AssetToolsModule.h"
#include "WALandscapeGraphAssetEditorToolbar.h"
#include "PropertyEditorModule.h"
#include "BlueprintEditorUtils.h"
#include "EdGraphUtilities.h"
#include "ScopedTransaction.h"
#include "PlatformApplicationMisc.h"
#include "WALandscapeGraph.h"
#include "WALandscapeGraphEdNode_Output.h"

#define LOCTEXT_NAMESPACE "GenericGraphAssetEditor"

const FName GenericGraphEditorAppName = FName(TEXT("LandscapeGraphEditor"));

struct FWALandscapeGraphAssetEditorTabs
{
	// Tab identifiers
	static const FName DetailsID;
	static const FName ViewportID;
};

//////////////////////////////////////////////////////////////////////////

const FName FWALandscapeGraphAssetEditorTabs::DetailsID(TEXT("Details"));
const FName FWALandscapeGraphAssetEditorTabs::ViewportID(TEXT("Viewport"));

//////////////////////////////////////////////////////////////////////////

FWALandscapeGraphAssetEditor::FWALandscapeGraphAssetEditor()
{
	EditingGraph = nullptr;

	OnPackageSavedDelegateHandle = UPackage::PackageSavedEvent.AddRaw(this, &FWALandscapeGraphAssetEditor::OnPackageSaved);
}

FWALandscapeGraphAssetEditor::~FWALandscapeGraphAssetEditor()
{
	UPackage::PackageSavedEvent.Remove(OnPackageSavedDelegateHandle);
}

void FWALandscapeGraphAssetEditor::InitGenericGraphAssetEditor(const EToolkitMode::Type Mode, const TSharedPtr< IToolkitHost >& InitToolkitHost, UWALandscapeGraph* Graph)
{
	EditingGraph = Graph;
	CreateEdGraph();

	FGenericCommands::Register();
	FGraphEditorCommands::Register();
	FWALandscapeGraphEditorCommands::Register();

	if (!ToolbarBuilder.IsValid())
	{
		ToolbarBuilder = MakeShareable(new FWALandscapeGraphAssetEditorToolbar(SharedThis(this)));
	}

	BindCommands();

	CreateInternalWidgets();

	TSharedPtr<FExtender> ToolbarExtender = MakeShareable(new FExtender);

	ToolbarBuilder->AddGenericGraphToolbar(ToolbarExtender);

	// Layout
	const TSharedRef<FTabManager::FLayout> StandaloneDefaultLayout = FTabManager::NewLayout("Standalone_GenericGraphEditor_Layout_v1")
		->AddArea
		(
			FTabManager::NewPrimaryArea()->SetOrientation(Orient_Vertical)
			->Split
			(
				FTabManager::NewStack()
				->SetSizeCoefficient(0.1f)
				->AddTab(GetToolbarTabId(), ETabState::OpenedTab)->SetHideTabWell(true)
			)
			->Split
			(
				FTabManager::NewSplitter()->SetOrientation(Orient_Horizontal)->SetSizeCoefficient(0.9f)
				->Split
				(
					FTabManager::NewStack()
					->SetSizeCoefficient(0.225f)
					->AddTab(FWALandscapeGraphAssetEditorTabs::ViewportID, ETabState::OpenedTab)
				)
				->Split
				(
					FTabManager::NewStack()
					->SetSizeCoefficient(0.65f)
					->AddTab(FWALandscapeGraphAssetEditorTabs::DetailsID, ETabState::OpenedTab)->SetHideTabWell(true)
				)
			)
		);

	const bool bCreateDefaultStandaloneMenu = true;
	const bool bCreateDefaultToolbar = true;
	FAssetEditorToolkit::InitAssetEditor(Mode, InitToolkitHost, GenericGraphEditorAppName, StandaloneDefaultLayout, bCreateDefaultStandaloneMenu, bCreateDefaultToolbar, EditingGraph, false);

	RegenerateMenusAndToolbars();
}

void FWALandscapeGraphAssetEditor::RegisterTabSpawners(const TSharedRef<FTabManager>& InTabManager)
{
	WorkspaceMenuCategory = InTabManager->AddLocalWorkspaceMenuCategory(LOCTEXT("WorkspaceMenu_SoundCueEditor", "Sound Cue Editor"));
	auto WorkspaceMenuCategoryRef = WorkspaceMenuCategory.ToSharedRef();

	FAssetEditorToolkit::RegisterTabSpawners(InTabManager);

	InTabManager->RegisterTabSpawner(FWALandscapeGraphAssetEditorTabs::ViewportID, FOnSpawnTab::CreateSP(this, &FWALandscapeGraphAssetEditor::SpawnTab_Viewport))
		.SetDisplayName(LOCTEXT("GraphCanvasTab", "Viewport"))
		.SetGroup(WorkspaceMenuCategoryRef)
		.SetIcon(FSlateIcon(FEditorStyle::GetStyleSetName(), "GraphEditor.EventGraph_16x"));

	InTabManager->RegisterTabSpawner(FWALandscapeGraphAssetEditorTabs::DetailsID, FOnSpawnTab::CreateSP(this, &FWALandscapeGraphAssetEditor::SpawnTab_Details))
		.SetDisplayName(LOCTEXT("DetailsTab", "Details"))
		.SetGroup(WorkspaceMenuCategoryRef)
		.SetIcon(FSlateIcon(FEditorStyle::GetStyleSetName(), "LevelEditor.Tabs.Details"));
}

void FWALandscapeGraphAssetEditor::UnregisterTabSpawners(const TSharedRef<FTabManager>& InTabManager)
{
	FAssetEditorToolkit::UnregisterTabSpawners(InTabManager);

	InTabManager->UnregisterTabSpawner(FWALandscapeGraphAssetEditorTabs::ViewportID);
	InTabManager->UnregisterTabSpawner(FWALandscapeGraphAssetEditorTabs::DetailsID);
}

FName FWALandscapeGraphAssetEditor::GetToolkitFName() const
{
	return FName("FGenericGraphEditor");
}

FText FWALandscapeGraphAssetEditor::GetBaseToolkitName() const
{
	return LOCTEXT("GenericGraphEditorAppLabel", "Generic Graph Editor");
}

FText FWALandscapeGraphAssetEditor::GetToolkitName() const
{
	const bool bDirtyState = EditingGraph->GetOutermost()->IsDirty();

	FFormatNamedArguments Args;
	Args.Add(TEXT("GenericGraphName"), FText::FromString(EditingGraph->GetName()));
	Args.Add(TEXT("DirtyState"), bDirtyState ? FText::FromString(TEXT("*")) : FText::GetEmpty());
	return FText::Format(LOCTEXT("GenericGraphEditorToolkitName", "{GenericGraphName}{DirtyState}"), Args);
}

FText FWALandscapeGraphAssetEditor::GetToolkitToolTipText() const
{
	return FAssetEditorToolkit::GetToolTipTextForObject(EditingGraph);
}

FLinearColor FWALandscapeGraphAssetEditor::GetWorldCentricTabColorScale() const
{
	return FLinearColor::White;
}

FString FWALandscapeGraphAssetEditor::GetWorldCentricTabPrefix() const
{
	return TEXT("GenericGraphEditor");
}

FString FWALandscapeGraphAssetEditor::GetDocumentationLink() const
{
	return TEXT("");
}

void FWALandscapeGraphAssetEditor::SaveAsset_Execute()
{
	if (EditingGraph != nullptr)
	{
		RebuildGenericGraph();
	}

	FAssetEditorToolkit::SaveAsset_Execute();
}

void FWALandscapeGraphAssetEditor::AddReferencedObjects(FReferenceCollector& Collector)
{
	Collector.AddReferencedObject(EditingGraph);
	Collector.AddReferencedObject(EditingGraph->EdGraph);
}

TSharedRef<SDockTab> FWALandscapeGraphAssetEditor::SpawnTab_Viewport(const FSpawnTabArgs& Args)
{
	check(Args.GetTabId() == FWALandscapeGraphAssetEditorTabs::ViewportID);

	TSharedRef<SDockTab> SpawnedTab = SNew(SDockTab)
		.Label(FText::FromString("Viewport"));

	if (ViewportWidget.IsValid())
	{
		SpawnedTab->SetContent(ViewportWidget.ToSharedRef());
	}

	return SpawnedTab;
}

TSharedRef<SDockTab> FWALandscapeGraphAssetEditor::SpawnTab_Details(const FSpawnTabArgs& Args)
{
	check(Args.GetTabId() == FWALandscapeGraphAssetEditorTabs::DetailsID);

	return SNew(SDockTab)
		.Icon(FEditorStyle::GetBrush("LevelEditor.Tabs.Details"))
		.Label(FText::FromString("Details"))
		[
			PropertyWidget.ToSharedRef()
		];
}

void FWALandscapeGraphAssetEditor::CreateInternalWidgets()
{
	ViewportWidget = CreateViewportWidget();

	FDetailsViewArgs Args;
	Args.bHideSelectionTip = true;
	Args.NotifyHook = this;

	FPropertyEditorModule& PropertyModule = FModuleManager::LoadModuleChecked<FPropertyEditorModule>("PropertyEditor");
	PropertyWidget = PropertyModule.CreateDetailView(Args);
	PropertyWidget->SetObject(EditingGraph);

	//PropertyWidget->SetIsPropertyEditingEnabledDelegate(FIsPropertyEditingEnabled::CreateSP(this, &FWALandscapeGraphAssetEditor::IsPropertyEditable));
	PropertyWidget->OnFinishedChangingProperties().AddSP(this, &FWALandscapeGraphAssetEditor::OnFinishedChangingProperties);
}

TSharedRef<SGraphEditor> FWALandscapeGraphAssetEditor::CreateViewportWidget()
{
	FGraphAppearanceInfo AppearanceInfo;
	AppearanceInfo.CornerText = FText::FromString("Landscape Graph");

	CreateCommandList();

	SGraphEditor::FGraphEditorEvents InEvents;
	InEvents.OnSelectionChanged = SGraphEditor::FOnSelectionChanged::CreateSP(this, &FWALandscapeGraphAssetEditor::OnSelectedNodesChanged);
	InEvents.OnNodeDoubleClicked = FSingleNodeEvent::CreateSP(this, &FWALandscapeGraphAssetEditor::OnNodeDoubleClicked);

	return SNew(SGraphEditor)
		.AdditionalCommands(GraphEditorCommands)
		.IsEditable(true)
		.Appearance(AppearanceInfo)
		.GraphToEdit(EditingGraph->EdGraph)
		.GraphEvents(InEvents)
		.AutoExpandActionMenu(true)
		.ShowGraphStateOverlay(false);
}


void FWALandscapeGraphAssetEditor::BindCommands()
{
	ToolkitCommands->MapAction(FWALandscapeGraphEditorCommands::Get().GraphSettings,
		FExecuteAction::CreateSP(this, &FWALandscapeGraphAssetEditor::GraphSettings),
		FCanExecuteAction::CreateSP(this, &FWALandscapeGraphAssetEditor::CanGraphSettings)
	);
	ToolkitCommands->MapAction(FWALandscapeGraphEditorCommands::Get().GenerateLandscape,
		FExecuteAction::CreateRaw(this, &FWALandscapeGraphAssetEditor::GenerateLandscape));
}

void FWALandscapeGraphAssetEditor::CreateEdGraph()
{
	if (EditingGraph->EdGraph == nullptr)
	{
		EditingGraph->EdGraph = CastChecked<UWALandscapeGraphEdGraph>(FBlueprintEditorUtils::CreateNewGraph(EditingGraph, NAME_None, UWALandscapeGraphEdGraph::StaticClass(), UWALandscapeGraphSchema::StaticClass()));
		EditingGraph->EdGraph->bAllowDeletion = false;

		// Give the schema a chance to fill out any required nodes (like the results node)
		const UEdGraphSchema* Schema = EditingGraph->EdGraph->GetSchema();
		Schema->CreateDefaultNodesForGraph(*EditingGraph->EdGraph);

		EditingGraph->Schema = Cast<UWALandscapeGraphSchema>(const_cast<UEdGraphSchema*>(Schema));
	}
}

void FWALandscapeGraphAssetEditor::CreateCommandList()
{
	if (GraphEditorCommands.IsValid())
	{
		return;
	}

	GraphEditorCommands = MakeShareable(new FUICommandList);

	// Can't use CreateSP here because derived editor are already implementing TSharedFromThis<FAssetEditorToolkit>
	// however it should be safe, since commands are being used only within this editor
	// if it ever crashes, this function will have to go away and be reimplemented in each derived class

	GraphEditorCommands->MapAction(FWALandscapeGraphEditorCommands::Get().GraphSettings,
		FExecuteAction::CreateRaw(this, &FWALandscapeGraphAssetEditor::GraphSettings),
		FCanExecuteAction::CreateRaw(this, &FWALandscapeGraphAssetEditor::CanGraphSettings));
	
	GraphEditorCommands->MapAction(FWALandscapeGraphEditorCommands::Get().GenerateLandscape,
		FExecuteAction::CreateRaw(this, &FWALandscapeGraphAssetEditor::GenerateLandscape));

	GraphEditorCommands->MapAction(FGenericCommands::Get().SelectAll,
		FExecuteAction::CreateRaw(this, &FWALandscapeGraphAssetEditor::SelectAllNodes),
		FCanExecuteAction::CreateRaw(this, &FWALandscapeGraphAssetEditor::CanSelectAllNodes)
	);

	GraphEditorCommands->MapAction(FGenericCommands::Get().Delete,
		FExecuteAction::CreateRaw(this, &FWALandscapeGraphAssetEditor::DeleteSelectedNodes),
		FCanExecuteAction::CreateRaw(this, &FWALandscapeGraphAssetEditor::CanDeleteNodes)
	);

	GraphEditorCommands->MapAction(FGenericCommands::Get().Copy,
		FExecuteAction::CreateRaw(this, &FWALandscapeGraphAssetEditor::CopySelectedNodes),
		FCanExecuteAction::CreateRaw(this, &FWALandscapeGraphAssetEditor::CanCopyNodes)
	);

	GraphEditorCommands->MapAction(FGenericCommands::Get().Cut,
		FExecuteAction::CreateRaw(this, &FWALandscapeGraphAssetEditor::CutSelectedNodes),
		FCanExecuteAction::CreateRaw(this, &FWALandscapeGraphAssetEditor::CanCutNodes)
	);

	GraphEditorCommands->MapAction(FGenericCommands::Get().Paste,
		FExecuteAction::CreateRaw(this, &FWALandscapeGraphAssetEditor::PasteNodes),
		FCanExecuteAction::CreateRaw(this, &FWALandscapeGraphAssetEditor::CanPasteNodes)
	);

	GraphEditorCommands->MapAction(FGenericCommands::Get().Duplicate,
		FExecuteAction::CreateRaw(this, &FWALandscapeGraphAssetEditor::DuplicateNodes),
		FCanExecuteAction::CreateRaw(this, &FWALandscapeGraphAssetEditor::CanDuplicateNodes)
	);
}

TSharedPtr<SGraphEditor> FWALandscapeGraphAssetEditor::GetCurrGraphEditor()
{
	return ViewportWidget;
}

FGraphPanelSelectionSet FWALandscapeGraphAssetEditor::GetSelectedNodes()
{
	FGraphPanelSelectionSet CurrentSelection;
	TSharedPtr<SGraphEditor> FocusedGraphEd = GetCurrGraphEditor();
	if (FocusedGraphEd.IsValid())
	{
		CurrentSelection = FocusedGraphEd->GetSelectedNodes();
	}

	return CurrentSelection;
}

void FWALandscapeGraphAssetEditor::RebuildGenericGraph()
{
	if (EditingGraph == nullptr)
	{
		//LOG_WARNING(TEXT("FWALandscapeGraphAssetEditor::RebuildGenericGraph EditingGraph is nullptr"));
		return;
	}

	UWALandscapeGraphEdGraph* EdGraph = Cast<UWALandscapeGraphEdGraph>(EditingGraph->EdGraph);
	check(EdGraph != nullptr);

	EdGraph->RebuildGenericGraph();
}

void FWALandscapeGraphAssetEditor::SelectAllNodes()
{
	TSharedPtr<SGraphEditor> CurrentGraphEditor = GetCurrGraphEditor();
	if (CurrentGraphEditor.IsValid())
	{
		CurrentGraphEditor->SelectAllNodes();
	}
}

bool FWALandscapeGraphAssetEditor::CanSelectAllNodes()
{
	return true;
}

void FWALandscapeGraphAssetEditor::DeleteSelectedNodes()
{
	TSharedPtr<SGraphEditor> CurrentGraphEditor = GetCurrGraphEditor();
	if (!CurrentGraphEditor.IsValid())
	{
		return;
	}

	const FScopedTransaction Transaction(FGenericCommands::Get().Delete->GetDescription());
	CurrentGraphEditor->GetCurrentGraph()->Modify();

	const FGraphPanelSelectionSet SelectedNodes = CurrentGraphEditor->GetSelectedNodes();
	CurrentGraphEditor->ClearSelectionSet();

	for (FGraphPanelSelectionSet::TConstIterator NodeIt(SelectedNodes); NodeIt; ++NodeIt)
	{
		if (UEdGraphNode* Node = Cast<UEdGraphNode>(*NodeIt))
		{
			if (Node->CanUserDeleteNode())
			{
				Node->Modify();
				Node->DestroyNode();
			}
		}
	}

	//LOG_WARNING(TEXT("FWALandscapeGraphAssetEditor::DeleteSelectedNodes Exec"));
}

bool FWALandscapeGraphAssetEditor::CanDeleteNodes()
{
	// If any of the nodes can be deleted then we should allow deleting
	const FGraphPanelSelectionSet SelectedNodes = GetSelectedNodes();
	for (FGraphPanelSelectionSet::TConstIterator SelectedIter(SelectedNodes); SelectedIter; ++SelectedIter)
	{
		UEdGraphNode* Node = Cast<UEdGraphNode>(*SelectedIter);
		if (Node != nullptr && Node->CanUserDeleteNode())
		{
			return true;
		}
	}

	//LOG_WARNING(TEXT("FWALandscapeGraphAssetEditor::CanDeleteNodes Can't delete"));

	return false;
}

void FWALandscapeGraphAssetEditor::DeleteSelectedDuplicatableNodes()
{
	TSharedPtr<SGraphEditor> CurrentGraphEditor = GetCurrGraphEditor();
	if (!CurrentGraphEditor.IsValid())
	{
		return;
	}

	const FGraphPanelSelectionSet OldSelectedNodes = CurrentGraphEditor->GetSelectedNodes();
	CurrentGraphEditor->ClearSelectionSet();

	for (FGraphPanelSelectionSet::TConstIterator SelectedIter(OldSelectedNodes); SelectedIter; ++SelectedIter)
	{
		UEdGraphNode* Node = Cast<UEdGraphNode>(*SelectedIter);
		if (Node && Node->CanDuplicateNode())
		{
			CurrentGraphEditor->SetNodeSelection(Node, true);
		}
	}

	// Delete the duplicatable nodes
	DeleteSelectedNodes();

	CurrentGraphEditor->ClearSelectionSet();

	for (FGraphPanelSelectionSet::TConstIterator SelectedIter(OldSelectedNodes); SelectedIter; ++SelectedIter)
	{
		if (UEdGraphNode* Node = Cast<UEdGraphNode>(*SelectedIter))
		{
			CurrentGraphEditor->SetNodeSelection(Node, true);
		}
	}
}

void FWALandscapeGraphAssetEditor::CutSelectedNodes()
{
	CopySelectedNodes();
	DeleteSelectedDuplicatableNodes();
}

bool FWALandscapeGraphAssetEditor::CanCutNodes()
{
	return CanCopyNodes() && CanDeleteNodes();
}

void FWALandscapeGraphAssetEditor::CopySelectedNodes()
{
	// Export the selected nodes and place the text on the clipboard
	FGraphPanelSelectionSet SelectedNodes = GetSelectedNodes();

	FString ExportedText;

	for (FGraphPanelSelectionSet::TIterator SelectedIter(SelectedNodes); SelectedIter; ++SelectedIter)
	{
		UEdGraphNode* Node = Cast<UEdGraphNode>(*SelectedIter);
		if (Node == nullptr)
		{
			SelectedIter.RemoveCurrent();
			continue;
		}

		Node->PrepareForCopying();
	}

	FEdGraphUtilities::ExportNodesToText(SelectedNodes, ExportedText);
	FPlatformApplicationMisc::ClipboardCopy(*ExportedText);
}

bool FWALandscapeGraphAssetEditor::CanCopyNodes()
{
	// If any of the nodes can be duplicated then we should allow copying
	const FGraphPanelSelectionSet SelectedNodes = GetSelectedNodes();
	for (FGraphPanelSelectionSet::TConstIterator SelectedIter(SelectedNodes); SelectedIter; ++SelectedIter)
	{
		UEdGraphNode* Node = Cast<UEdGraphNode>(*SelectedIter);
		if (Node && Node->CanDuplicateNode())
		{
			return true;
		}
	}

	return false;
}

void FWALandscapeGraphAssetEditor::PasteNodes()
{
	TSharedPtr<SGraphEditor> CurrentGraphEditor = GetCurrGraphEditor();
	if (CurrentGraphEditor.IsValid())
	{
		PasteNodesHere(CurrentGraphEditor->GetPasteLocation());
	}
}

void FWALandscapeGraphAssetEditor::PasteNodesHere(const FVector2D& Location)
{
}

bool FWALandscapeGraphAssetEditor::CanPasteNodes()
{
	return false;
}

void FWALandscapeGraphAssetEditor::DuplicateNodes()
{
	CopySelectedNodes();
	PasteNodes();
}

bool FWALandscapeGraphAssetEditor::CanDuplicateNodes()
{
	//return CanCopyNodes();
	return false;
}

void FWALandscapeGraphAssetEditor::GraphSettings()
{
	PropertyWidget->SetObject(EditingGraph);
	if (EditingGraph->LandscapeOutput)
	{
		EditingGraph->LandscapeOutput->GenerateHeightmap();
	}
	//LOG_WARNING(TEXT("FWALandscapeGraphAssetEditor::GraphSettings"));
}

void FWALandscapeGraphAssetEditor::GenerateLandscape()
{
	
	if (const UWALandscapeGraphSchema* Schema = Cast<UWALandscapeGraphSchema>(EditingGraph->EdGraph->GetSchema()))
	{
		TArray<uint16> out;
		Schema->Output->GenerateHeightMap(out);
	}
	//LOG_WARNING(TEXT("FWALandscapeGraphAssetEditor::GraphSettings"));
}

bool FWALandscapeGraphAssetEditor::CanGraphSettings() const
{
	return true;
}

void FWALandscapeGraphAssetEditor::OnSelectedNodesChanged(const TSet<class UObject*>& NewSelection)
{
	TArray<UObject*> Selection;

	for (UObject* SelectionEntry : NewSelection)
	{
		Selection.Add(SelectionEntry);
	}

	if (Selection.Num() == 0) 
	{
		PropertyWidget->SetObject(EditingGraph);

	}
	else if (Selection.Num() == 1)
	{
		PropertyWidget->SetObject(Selection[0]);
	}
	else
	{
		PropertyWidget->SetObject(nullptr);
	}
}

void FWALandscapeGraphAssetEditor::OnNodeDoubleClicked(UEdGraphNode* Node)
{
	
}

void FWALandscapeGraphAssetEditor::OnFinishedChangingProperties(const FPropertyChangedEvent& PropertyChangedEvent)
{
	if (EditingGraph == nullptr)
		return;

	RebuildGenericGraph();

	EditingGraph->EdGraph->GetSchema()->ForceVisualizationCacheClear();
}

void FWALandscapeGraphAssetEditor::OnPackageSaved(const FString& PackageFileName, UObject* Outer)
{
	RebuildGenericGraph();
}

void FWALandscapeGraphAssetEditor::RegisterToolbarTab(const TSharedRef<class FTabManager>& InTabManager) 
{
	FAssetEditorToolkit::RegisterTabSpawners(InTabManager);
}


#undef LOCTEXT_NAMESPACE

