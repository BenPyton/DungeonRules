#include "DungeonRulesToolkit.h"
#include "DungeonRulesEdLog.h"
#include "DungeonRulesEdTypes.h"
#include "PropertyEditorModule.h"
#include "Modules/ModuleManager.h"
#include "Slate.h"
#include "Kismet2/BlueprintEditorUtils.h"
#include "GraphEditorActions.h"
#include "DungeonRules.h"
#include "DungeonRulesGraph.h"
#include "DungeonRulesSchema.h"
#include "EdGraphUtilities.h"
#include "HAL/PlatformApplicationMisc.h"
#include "Nodes/RuleNodeBase.h"
#include "Editor.h"
#include "Misc/EngineVersionComparison.h"

#define LOCTEXT_NAMESPACE "DungeonRulesEditor"

const FName FDungeonRulesToolkit::DetailsTabId(TEXT("DungeonRulesEditorDetailsTabId"));
const FName FDungeonRulesToolkit::GraphTabId(TEXT("DungeonRulesEditorGraphTabId"));

FDungeonRulesToolkit::FDungeonRulesToolkit()
{
	UEditorEngine* Editor = (UEditorEngine*)GEngine;
	if (Editor)
	{
		Editor->RegisterForUndo(this);
	}

	//OnClassListUpdatedDelegateHandle = FGraphNodeClassHelper::OnPackageListUpdated.AddRaw(this, &FDungeonRulesToolkit::OnClassListUpdated);

	//GEditor->OnBlueprintCompiled().AddRaw(this, &FDungeonRulesToolkit::BlueprintCompiled);
}

FDungeonRulesToolkit::~FDungeonRulesToolkit()
{
	//GEditor->OnBlueprintCompiled().RemoveAll(this);

	UEditorEngine* Editor = (UEditorEngine*)GEngine;
	if (Editor)
	{
		Editor->UnregisterForUndo(this);
	}

	//FGraphNodeClassHelper::OnPackageListUpdated.Remove(OnClassListUpdatedDelegateHandle);
}

void FDungeonRulesToolkit::CreateCommandList()
{
	if (GraphEditorCommands.IsValid())
	{
		return;
	}

	GraphEditorCommands = MakeShareable(new FUICommandList);

	// Can't use CreateSP here because derived editor are already implementing TSharedFromThis<FAssetEditorToolkit>
	// however it should be safe, since commands are being used only within this editor
	// if it ever crashes, this function will have to go away and be reimplemented in each derived class

	GraphEditorCommands->MapAction(FGenericCommands::Get().SelectAll,
		FExecuteAction::CreateRaw(this, &FDungeonRulesToolkit::SelectAllNodes),
		FCanExecuteAction::CreateRaw(this, &FDungeonRulesToolkit::CanSelectAllNodes)
	);

	GraphEditorCommands->MapAction(FGenericCommands::Get().Delete,
		FExecuteAction::CreateRaw(this, &FDungeonRulesToolkit::DeleteSelectedNodes),
		FCanExecuteAction::CreateRaw(this, &FDungeonRulesToolkit::CanDeleteNodes)
	);

	GraphEditorCommands->MapAction(FGenericCommands::Get().Copy,
		FExecuteAction::CreateRaw(this, &FDungeonRulesToolkit::CopySelectedNodes),
		FCanExecuteAction::CreateRaw(this, &FDungeonRulesToolkit::CanCopyNodes)
	);

	GraphEditorCommands->MapAction(FGenericCommands::Get().Cut,
		FExecuteAction::CreateRaw(this, &FDungeonRulesToolkit::CutSelectedNodes),
		FCanExecuteAction::CreateRaw(this, &FDungeonRulesToolkit::CanCutNodes)
	);

	GraphEditorCommands->MapAction(FGenericCommands::Get().Paste,
		FExecuteAction::CreateRaw(this, &FDungeonRulesToolkit::PasteNodes),
		FCanExecuteAction::CreateRaw(this, &FDungeonRulesToolkit::CanPasteNodes)
	);

	GraphEditorCommands->MapAction(FGenericCommands::Get().Duplicate,
		FExecuteAction::CreateRaw(this, &FDungeonRulesToolkit::DuplicateNodes),
		FCanExecuteAction::CreateRaw(this, &FDungeonRulesToolkit::CanDuplicateNodes)
	);

	GraphEditorCommands->MapAction(
		FGraphEditorCommands::Get().CreateComment,
		FExecuteAction::CreateRaw(this, &FDungeonRulesToolkit::OnCreateComment),
		FCanExecuteAction::CreateRaw(this, &FDungeonRulesToolkit::CanCreateComment)
	);
}

FGraphPanelSelectionSet FDungeonRulesToolkit::GetSelectedNodes() const
{
	FGraphPanelSelectionSet CurrentSelection;
	if (TSharedPtr<SGraphEditor> FocusedGraphEd = UpdateGraphEdPtr.Pin())
	{
		CurrentSelection = FocusedGraphEd->GetSelectedNodes();
	}

	return CurrentSelection;
}

void FDungeonRulesToolkit::PostUndo(bool bSuccess)
{
	if (bSuccess)
	{
		// Clear selection, to avoid holding refs to nodes that go away
		if (TSharedPtr<SGraphEditor> CurrentGraphEditor = UpdateGraphEdPtr.Pin())
		{
			CurrentGraphEditor->ClearSelectionSet();
			CurrentGraphEditor->NotifyGraphChanged();
		}
		FSlateApplication::Get().DismissAllMenus();
	}
}

void FDungeonRulesToolkit::PostRedo(bool bSuccess)
{
	if (bSuccess)
	{
		// Clear selection, to avoid holding refs to nodes that go away
		if (TSharedPtr<SGraphEditor> CurrentGraphEditor = UpdateGraphEdPtr.Pin())
		{
			CurrentGraphEditor->ClearSelectionSet();
			CurrentGraphEditor->NotifyGraphChanged();
		}
		FSlateApplication::Get().DismissAllMenus();
	}
}


TSharedRef<SDockTab> FDungeonRulesToolkit::SpawnTab_Properties(const FSpawnTabArgs& Args)
{
	check(Args.GetTabId() == DetailsTabId);

	return SNew(SDockTab)
		.Icon(FStyleProvider::GetBrush("LevelEditor.Tabs.Details"))
		.Label(LOCTEXT("DungeonRulesDetailsTitle", "Details"))
		.TabRole(ETabRole::PanelTab)
		[
			DetailsWidget.ToSharedRef()
		];
}

TSharedRef<SDockTab> FDungeonRulesToolkit::SpawnTab_GraphCanvas(const FSpawnTabArgs& Args)
{
	check(Args.GetTabId() == GraphTabId);

	//if (DungeonRules->EdGraph != nullptr)
	//{
	//	EdGraphEditor = SNew(SGraphEditor)
	//		.AdditionalCommands(GraphEditorCommands)
	//		.GraphToEdit(DungeonRules->EdGraph);
	//}
	//else
	//{
	//	EDELog("There is no editor graph inside the graph asset.");
	//}

	TSharedRef<SDockTab> SpawnedTab = SNew(SDockTab)
		.Label(LOCTEXT("DungeonRulesGraphCanvasTitle", "Viewport"))
		.TabRole(ETabRole::PanelTab);

	if (EdGraphEditor.IsValid())
	{
		SpawnedTab->SetContent(EdGraphEditor.ToSharedRef());
	}
	return SpawnedTab;
}

FLinearColor FDungeonRulesToolkit::GetWorldCentricTabColorScale() const
{
	return FLinearColor(0.3f, 0.2f, 0.5f, 0.5f);
}

FName FDungeonRulesToolkit::GetToolkitFName() const
{
	return FName("Dungeon Rules");
}

FText FDungeonRulesToolkit::GetBaseToolkitName() const
{
	return LOCTEXT("AppLabel", "DungeonRules");
}

FString FDungeonRulesToolkit::GetWorldCentricTabPrefix() const
{
	return LOCTEXT("WorldCentricTabPrefix", "DungeonRules ").ToString();
}

FText FDungeonRulesToolkit::GetToolkitName() const
{
	return (DungeonRules != nullptr) ? FAssetEditorToolkit::GetLabelForObject(DungeonRules) : FText();
}

FText FDungeonRulesToolkit::GetToolkitToolTipText() const
{
	return (DungeonRules != nullptr) ? FAssetEditorToolkit::GetToolTipTextForObject(DungeonRules) : FText();
}

void FDungeonRulesToolkit::InitGraphAssetEditor(const EToolkitMode::Type InMode, const TSharedPtr<class IToolkitHost>& InToolkitHost, UObject* ObjectToEdit)
{
	DungeonRules = CastChecked<UDungeonRules>(ObjectToEdit);

	// Support undo/redo
	//DungeonRules->SetFlags(RF_Transactional);
	//GEditor->RegisterForUndo(this);

	if (DungeonRules->EdGraph == nullptr)
	{
		DungeonEd_LogInfo("Creating a new Dungeon Rules graph.");
		DungeonRules->EdGraph = CastChecked<UDungeonRulesGraph>(FBlueprintEditorUtils::CreateNewGraph(DungeonRules, NAME_None, UDungeonRulesGraph::StaticClass(), UDungeonRulesSchema::StaticClass()));
		//DungeonRules->EdGraph->bAllowDeletion = false;

		//Give the schema a chance to fill out any required nodes (like the root node)
		const UEdGraphSchema* Schema = DungeonRules->EdGraph->GetSchema();
		Schema->CreateDefaultNodesForGraph(*DungeonRules->EdGraph);
	}

	FGenericCommands::Register();
	FGraphEditorCommands::Register();
	CreateCommandList();

	CreateInternalWidgets();

	const TSharedRef<FTabManager::FLayout> StandaloneDefaultLayout = FTabManager::NewLayout("Standalone_DungeonRulesEditor_Layout")
		->AddArea
		(
			FTabManager::NewPrimaryArea()
			->SetOrientation(Orient_Horizontal)
			->Split
			(
				FTabManager::NewStack()
				->SetSizeCoefficient(0.225f)
				->AddTab(DetailsTabId, ETabState::OpenedTab)
			)
			->Split
			(
				FTabManager::NewStack()
				->SetSizeCoefficient(0.65f)
				->AddTab(GraphTabId, ETabState::OpenedTab)
				->SetHideTabWell(true)
			)
		);

	const bool bCreateDefaultStandaloneMenu = true;
	const bool bCreateDefaultToolbar = true;
	FAssetEditorToolkit::InitAssetEditor(InMode, InToolkitHost, TEXT("DungeonRulesEditorApp"), StandaloneDefaultLayout, bCreateDefaultStandaloneMenu, bCreateDefaultToolbar, ObjectToEdit, false);
}

void FDungeonRulesToolkit::CreateInternalWidgets()
{
	EdGraphEditor = CreateGraphEditorWidget();
	UpdateGraphEdPtr = EdGraphEditor;

	//FDetailsViewArgs Args;
	//Args.bHideSelectionTip = true;
	//Args.NotifyHook = this;
	//
	//FPropertyEditorModule& PropertyModule = FModuleManager::LoadModuleChecked<FPropertyEditorModule>("PropertyEditor");
	//DungeonRulesProperties = PropertyModule.CreateDetailView(Args);
	//DungeonRulesProperties->SetObject(DungeonRules);

	FDetailsViewArgs DetailsViewArgs;
	DetailsViewArgs.bUpdatesFromSelection = false;
	DetailsViewArgs.bCustomNameAreaLocation = false;
	DetailsViewArgs.bLockable = false;
	DetailsViewArgs.NameAreaSettings = FDetailsViewArgs::HideNameArea;
	DetailsViewArgs.NotifyHook = this;

	FPropertyEditorModule& PropertyEditorModule = FModuleManager::LoadModuleChecked<FPropertyEditorModule>("PropertyEditor");
	DetailsWidget = PropertyEditorModule.CreateDetailView(DetailsViewArgs);
	DetailsWidget->SetObject(DungeonRules);

	//Palette = SNew(SSoundCuePalette);
}

/*
void FDungeonRulesToolkit::BlueprintCompiled()
{
	UEdGraph* EdGraph = EdGraphEditor->GetCurrentGraph();
	if (UDungeonRulesGraph* MyGraph = Cast<UDungeonRulesGraph>(EdGraph))
		MyGraph->RefreshNodes();
}
*/

void FDungeonRulesToolkit::SaveAsset_Execute()
{
	if (DungeonRules)
	{
		UDungeonRulesGraph* EdGraph = Cast<UDungeonRulesGraph>(DungeonRules->EdGraph);
		if (EdGraph)
		{
			EdGraph->OnSave();
		}
	}
	// save it
	FAssetEditorToolkit::SaveAsset_Execute();
}

void FDungeonRulesToolkit::RegisterTabSpawners(const TSharedRef<FTabManager>& InTabManager)
{
	WorkspaceMenuCategory = InTabManager->AddLocalWorkspaceMenuCategory(LOCTEXT("WorkspaceMenu_DungeonRulesEditor", "Dungeon Rules Editor"));
	auto WorkspaceMenuCategoryRef = WorkspaceMenuCategory.ToSharedRef();

	FAssetEditorToolkit::RegisterTabSpawners(InTabManager);

	InTabManager->RegisterTabSpawner(DetailsTabId, FOnSpawnTab::CreateSP(this, &FDungeonRulesToolkit::SpawnTab_Properties))
		.SetDisplayName(LOCTEXT("DetailsTab", "Details"))
		.SetGroup(WorkspaceMenuCategoryRef)
		.SetIcon(FSlateIcon(STYLESET_NAME(), "LevelEditor.Tabs.Details"));

	InTabManager->RegisterTabSpawner(GraphTabId, FOnSpawnTab::CreateSP(this, &FDungeonRulesToolkit::SpawnTab_GraphCanvas))
		.SetDisplayName(LOCTEXT("GraphCanvasTab", "Viewport"))
		.SetGroup(WorkspaceMenuCategoryRef)
		.SetIcon(FSlateIcon(STYLESET_NAME(), "GraphEditor.EventGraph_16x"));
}

void FDungeonRulesToolkit::UnregisterTabSpawners(const TSharedRef<FTabManager>& InTabManager)
{
	FAssetEditorToolkit::UnregisterTabSpawners(InTabManager);
	InTabManager->UnregisterTabSpawner(DetailsTabId);
	InTabManager->UnregisterTabSpawner(GraphTabId);
}

TSharedRef<SGraphEditor> FDungeonRulesToolkit::CreateGraphEditorWidget()
{
	FGraphAppearanceInfo AppearanceInfo;
	AppearanceInfo.CornerText = LOCTEXT("AppearanceCornerText_DungeonRules", "DUNGEON RULES");

	SGraphEditor::FGraphEditorEvents InEvents;
	InEvents.OnSelectionChanged = SGraphEditor::FOnSelectionChanged::CreateSP(this, &FDungeonRulesToolkit::OnSelectedNodesChanged);
	//InEvents.OnTextCommitted = FOnNodeTextCommitted::CreateSP(this, &FDungeonRulesToolkit::OnNodeTitleCommitted);
	//InEvents.OnNodeDoubleClicked = FSingleNodeEvent::CreateSP(this, &FDungeonRulesToolkit::PlaySingleNode);

	return SNew(SGraphEditor)
		.AdditionalCommands(GraphEditorCommands)
		.IsEditable(true)
		.Appearance(AppearanceInfo)
		.GraphToEdit(DungeonRules->EdGraph)
		.GraphEvents(InEvents)
		.AutoExpandActionMenu(true)
		.ShowGraphStateOverlay(false);
}


void FDungeonRulesToolkit::OnSelectedNodesChanged(const TSet<class UObject*>& NewSelection)
{
	TArray<UObject*> Selection;

	if (NewSelection.Num())
	{
		for (TSet<class UObject*>::TConstIterator SetIt(NewSelection); SetIt; ++SetIt)
		{
#if false
			if (URuleNodeBase* GraphNode = Cast<URuleNodeBase>(*SetIt))
			{
				Selection.Add(GraphNode->NodeInstance);
			}
			else
#endif
			{
				Selection.Add(*SetIt);
			}
		}
		//Selection = NewSelection.Array();
	}
	else
	{
		Selection.Add(DungeonRules);
	}

	if (DetailsWidget.IsValid())
	{
		DetailsWidget->SetObjects(Selection);
	}
	//SetSelection(Selection);
}


void FDungeonRulesToolkit::SelectAllNodes()
{
	if (TSharedPtr<SGraphEditor> CurrentGraphEditor = UpdateGraphEdPtr.Pin())
	{
		CurrentGraphEditor->SelectAllNodes();
	}
}

bool FDungeonRulesToolkit::CanSelectAllNodes() const
{
	return true;
}

void FDungeonRulesToolkit::DeleteSelectedNodes()
{
	TSharedPtr<SGraphEditor> CurrentGraphEditor = UpdateGraphEdPtr.Pin();
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
}

bool FDungeonRulesToolkit::CanDeleteNodes() const
{
	// If any of the nodes can be deleted then we should allow deleting
	const FGraphPanelSelectionSet SelectedNodes = GetSelectedNodes();
	for (FGraphPanelSelectionSet::TConstIterator SelectedIter(SelectedNodes); SelectedIter; ++SelectedIter)
	{
		UEdGraphNode* Node = Cast<UEdGraphNode>(*SelectedIter);
		if (Node && Node->CanUserDeleteNode())
		{
			return true;
		}
	}

	return false;
}

void FDungeonRulesToolkit::DeleteSelectedDuplicatableNodes()
{
	TSharedPtr<SGraphEditor> CurrentGraphEditor = UpdateGraphEdPtr.Pin();
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

void FDungeonRulesToolkit::CutSelectedNodes()
{
	CopySelectedNodes();
	DeleteSelectedDuplicatableNodes();
}

bool FDungeonRulesToolkit::CanCutNodes() const
{
	return CanCopyNodes() && CanDeleteNodes();
}

void FDungeonRulesToolkit::CopySelectedNodes()
{
#if false
	// Export the selected nodes and place the text on the clipboard
	FGraphPanelSelectionSet SelectedNodes = GetSelectedNodes();
	TArray<UAIGraphNode*> SubNodes;

	FString ExportedText;

	int32 CopySubNodeIndex = 0;
	for (FGraphPanelSelectionSet::TIterator SelectedIter(SelectedNodes); SelectedIter; ++SelectedIter)
	{
		UEdGraphNode* Node = Cast<UEdGraphNode>(*SelectedIter);
		UAIGraphNode* AINode = Cast<UAIGraphNode>(Node);
		if (Node == nullptr)
		{
			SelectedIter.RemoveCurrent();
			continue;
		}

		Node->PrepareForCopying();

		if (AINode)
		{
			AINode->CopySubNodeIndex = CopySubNodeIndex;

			// append all subnodes for selection
			for (int32 Idx = 0; Idx < AINode->SubNodes.Num(); Idx++)
			{
				AINode->SubNodes[Idx]->CopySubNodeIndex = CopySubNodeIndex;
				SubNodes.Add(AINode->SubNodes[Idx]);
			}

			CopySubNodeIndex++;
		}
	}

	for (int32 Idx = 0; Idx < SubNodes.Num(); Idx++)
	{
		SelectedNodes.Add(SubNodes[Idx]);
		SubNodes[Idx]->PrepareForCopying();
	}

	FEdGraphUtilities::ExportNodesToText(SelectedNodes, ExportedText);
	FPlatformApplicationMisc::ClipboardCopy(*ExportedText);

	for (FGraphPanelSelectionSet::TIterator SelectedIter(SelectedNodes); SelectedIter; ++SelectedIter)
	{
		UAIGraphNode* Node = Cast<UAIGraphNode>(*SelectedIter);
		if (Node)
		{
			Node->PostCopyNode();
		}
	}
#endif
}

bool FDungeonRulesToolkit::CanCopyNodes() const
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

void FDungeonRulesToolkit::PasteNodes()
{
	if (TSharedPtr<SGraphEditor> CurrentGraphEditor = UpdateGraphEdPtr.Pin())
	{
		PasteNodesHere(CurrentGraphEditor->GetPasteLocation());
	}
}

void FDungeonRulesToolkit::PasteNodesHere(const FVector2D& Location)
{
#if false
	TSharedPtr<SGraphEditor> CurrentGraphEditor = UpdateGraphEdPtr.Pin();
	if (!CurrentGraphEditor.IsValid())
	{
		return;
	}

	// Undo/Redo support
	const FScopedTransaction Transaction(FGenericCommands::Get().Paste->GetDescription());
	UEdGraph* EdGraph = CurrentGraphEditor->GetCurrentGraph();
	UAIGraph* AIGraph = Cast<UAIGraph>(EdGraph);

	EdGraph->Modify();
	if (AIGraph)
	{
		AIGraph->LockUpdates();
	}

	UAIGraphNode* SelectedParent = NULL;
	bool bHasMultipleNodesSelected = false;

	const FGraphPanelSelectionSet SelectedNodes = GetSelectedNodes();
	for (FGraphPanelSelectionSet::TConstIterator SelectedIter(SelectedNodes); SelectedIter; ++SelectedIter)
	{
		UAIGraphNode* Node = Cast<UAIGraphNode>(*SelectedIter);
		if (Node && Node->IsSubNode())
		{
			Node = Node->ParentNode;
		}

		if (Node)
		{
			if (SelectedParent == nullptr)
			{
				SelectedParent = Node;
			}
			else
			{
				bHasMultipleNodesSelected = true;
				break;
			}
		}
	}

	// Clear the selection set (newly pasted stuff will be selected)
	CurrentGraphEditor->ClearSelectionSet();

	// Grab the text to paste from the clipboard.
	FString TextToImport;
	FPlatformApplicationMisc::ClipboardPaste(TextToImport);

	// Import the nodes
	TSet<UEdGraphNode*> PastedNodes;
	FEdGraphUtilities::ImportNodesFromText(EdGraph, TextToImport, /*out*/ PastedNodes);

	//Average position of nodes so we can move them while still maintaining relative distances to each other
	FVector2D AvgNodePosition(0.0f, 0.0f);

	// Number of nodes used to calculate AvgNodePosition
	int32 AvgCount = 0;

	for (TSet<UEdGraphNode*>::TIterator It(PastedNodes); It; ++It)
	{
		UEdGraphNode* EdNode = *It;
		UAIGraphNode* AINode = Cast<UAIGraphNode>(EdNode);
		if (EdNode && (AINode == nullptr || !AINode->IsSubNode()))
		{
			AvgNodePosition.X += EdNode->NodePosX;
			AvgNodePosition.Y += EdNode->NodePosY;
			++AvgCount;
		}
	}

	if (AvgCount > 0)
	{
		float InvNumNodes = 1.0f / float(AvgCount);
		AvgNodePosition.X *= InvNumNodes;
		AvgNodePosition.Y *= InvNumNodes;
	}

	bool bPastedParentNode = false;

	TMap<FGuid/*New*/, FGuid/*Old*/> NewToOldNodeMapping;

	TMap<int32, UAIGraphNode*> ParentMap;
	for (TSet<UEdGraphNode*>::TIterator It(PastedNodes); It; ++It)
	{
		UEdGraphNode* PasteNode = *It;
		UAIGraphNode* PasteAINode = Cast<UAIGraphNode>(PasteNode);

		if (PasteNode && (PasteAINode == nullptr || !PasteAINode->IsSubNode()))
		{
			bPastedParentNode = true;

			// Select the newly pasted stuff
			CurrentGraphEditor->SetNodeSelection(PasteNode, true);

			const FVector::FReal NodePosX = (PasteNode->NodePosX - AvgNodePosition.X) + Location.X;
			const FVector::FReal NodePosY = (PasteNode->NodePosY - AvgNodePosition.Y) + Location.Y;

			PasteNode->NodePosX = static_cast<int32>(NodePosX);
			PasteNode->NodePosY = static_cast<int32>(NodePosY);

			PasteNode->SnapToGrid(16);

			const FGuid OldGuid = PasteNode->NodeGuid;

			// Give new node a different Guid from the old one
			PasteNode->CreateNewGuid();

			const FGuid NewGuid = PasteNode->NodeGuid;

			NewToOldNodeMapping.Add(NewGuid, OldGuid);

			if (PasteAINode)
			{
				PasteAINode->RemoveAllSubNodes();
				ParentMap.Add(PasteAINode->CopySubNodeIndex, PasteAINode);
			}
		}
	}

	for (TSet<UEdGraphNode*>::TIterator It(PastedNodes); It; ++It)
	{
		UAIGraphNode* PasteNode = Cast<UAIGraphNode>(*It);
		if (PasteNode && PasteNode->IsSubNode())
		{
			PasteNode->NodePosX = 0;
			PasteNode->NodePosY = 0;

			// remove subnode from graph, it will be referenced from parent node
			PasteNode->DestroyNode();

			PasteNode->ParentNode = ParentMap.FindRef(PasteNode->CopySubNodeIndex);
			if (PasteNode->ParentNode)
			{
				PasteNode->ParentNode->AddSubNode(PasteNode, EdGraph);
			}
			else if (!bHasMultipleNodesSelected && !bPastedParentNode && SelectedParent)
			{
				PasteNode->ParentNode = SelectedParent;
				SelectedParent->AddSubNode(PasteNode, EdGraph);
			}
		}
	}

	FixupPastedNodes(PastedNodes, NewToOldNodeMapping);

	if (AIGraph)
	{
		AIGraph->UpdateClassData();
		AIGraph->OnNodesPasted(TextToImport);
		AIGraph->UnlockUpdates();
	}

	// Update UI
	CurrentGraphEditor->NotifyGraphChanged();

	UObject* GraphOwner = EdGraph->GetOuter();
	if (GraphOwner)
	{
		GraphOwner->PostEditChange();
		GraphOwner->MarkPackageDirty();
	}
#endif
}

#if false
void FDungeonRulesToolkit::FixupPastedNodes(const TSet<UEdGraphNode*>& PastedGraphNodes, const TMap<FGuid/*New*/, FGuid/*Old*/>& NewToOldNodeMapping)
{

}
#endif

bool FDungeonRulesToolkit::CanPasteNodes() const
{
	TSharedPtr<SGraphEditor> CurrentGraphEditor = UpdateGraphEdPtr.Pin();
	if (!CurrentGraphEditor.IsValid())
	{
		return false;
	}

	FString ClipboardContent;
	FPlatformApplicationMisc::ClipboardPaste(ClipboardContent);

	return FEdGraphUtilities::CanImportNodesFromText(CurrentGraphEditor->GetCurrentGraph(), ClipboardContent);
}

void FDungeonRulesToolkit::DuplicateNodes()
{
	CopySelectedNodes();
	PasteNodes();
}

bool FDungeonRulesToolkit::CanDuplicateNodes() const
{
	return CanCopyNodes();
}

bool FDungeonRulesToolkit::CanCreateComment() const
{
	TSharedPtr<SGraphEditor> CurrentGraphEditor = UpdateGraphEdPtr.Pin();
	return CurrentGraphEditor.IsValid() ? (CurrentGraphEditor->GetNumberOfSelectedNodes() != 0) : false;
}

void FDungeonRulesToolkit::OnCreateComment()
{
	TSharedPtr<SGraphEditor> CurrentGraphEditor = UpdateGraphEdPtr.Pin();
	if (UEdGraph* EdGraph = CurrentGraphEditor.IsValid() ? CurrentGraphEditor->GetCurrentGraph() : nullptr)
	{
		TSharedPtr<FEdGraphSchemaAction> Action = EdGraph->GetSchema()->GetCreateCommentAction();
		if (Action.IsValid())
		{
			Action->PerformAction(EdGraph, nullptr, FVector2D());
		}
	}
}

#undef LOCTEXT_NAMESPACE
