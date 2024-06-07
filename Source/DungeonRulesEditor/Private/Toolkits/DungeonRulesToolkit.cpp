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
#include "Nodes/RuleNode.h"
#include "Nodes/RuleTransitionNode.h"
#include "Editor.h"
#include "Misc/EngineVersionComparison.h"
#include "SNodePanel.h" // GetSnapGridSize
#include "DetailCustomizations/DungeonDetailsCustomization.h"

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

	GraphEditorCommands->MapAction(FGraphEditorCommands::Get().CreateComment,
		FExecuteAction::CreateRaw(this, &FDungeonRulesToolkit::OnCreateComment),
		FCanExecuteAction::CreateRaw(this, &FDungeonRulesToolkit::CanCreateComment)
	);

	GraphEditorCommands->MapAction(FGenericCommands::Get().Rename,
		FExecuteAction::CreateSP(this, &FDungeonRulesToolkit::OnRenameNode),
		FCanExecuteAction::CreateSP(this, &FDungeonRulesToolkit::CanRenameNodes)
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

	FDetailsViewArgs DetailsViewArgs;
	DetailsViewArgs.bUpdatesFromSelection = false;
	DetailsViewArgs.bCustomNameAreaLocation = false;
	DetailsViewArgs.bLockable = false;
	DetailsViewArgs.NameAreaSettings = FDetailsViewArgs::HideNameArea;
	DetailsViewArgs.NotifyHook = this;

	FPropertyEditorModule& PropertyEditorModule = FModuleManager::LoadModuleChecked<FPropertyEditorModule>("PropertyEditor");
	DetailsWidget = PropertyEditorModule.CreateDetailView(DetailsViewArgs);
	DetailsWidget->SetObject(DungeonRules);
	DetailsWidget->RegisterInstancedCustomPropertyLayout(UEdGraphNode::StaticClass(), FOnGetDetailCustomizationInstance::CreateStatic(&FEditorGraphNodeNameDetails::MakeInstance, this));
	DetailsWidget->RegisterInstancedCustomPropertyLayout(URuleNodeBase::StaticClass(), FOnGetDetailCustomizationInstance::CreateStatic(&FDungeonRuleNodeBaseDetails::MakeInstance));
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
	InEvents.OnVerifyTextCommit = FOnNodeVerifyTextCommit::CreateSP(this, &FDungeonRulesToolkit::OnNodeVerifyTitleCommit);
	InEvents.OnTextCommitted = FOnNodeTextCommitted::CreateSP(this, &FDungeonRulesToolkit::OnNodeTitleCommitted);
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
		for (UObject* SelectedObj : NewSelection)
		{
#if false
			if (URuleNodeBase* GraphNode = Cast<URuleNodeBase>(SelectedObj))
			{
				Selection.Add(GraphNode->GetNodeInstance());
			}
			else
#endif
			{
				Selection.Add(SelectedObj);
			}
		}
	}
	else
	{
		// No need for now
		//Selection.Add(DungeonRules);
	}

	if (DetailsWidget.IsValid())
	{
		DetailsWidget->SetObjects(Selection);
	}
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

	for (UObject* SelectedObj : SelectedNodes)
	{
		if (UEdGraphNode* Node = Cast<UEdGraphNode>(SelectedObj))
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

	for (UObject* SelectedObj : OldSelectedNodes)
	{
		UEdGraphNode* Node = Cast<UEdGraphNode>(SelectedObj);
		if (Node && Node->CanDuplicateNode())
		{
			CurrentGraphEditor->SetNodeSelection(Node, true);
		}
	}

	const FGraphPanelSelectionSet DeletedNodes = CurrentGraphEditor->GetSelectedNodes();

	// Delete the duplicatable nodes
	DeleteSelectedNodes();

	CurrentGraphEditor->ClearSelectionSet();

	for (UObject* SelectedObj : OldSelectedNodes)
	{
		if (DeletedNodes.Contains(SelectedObj))
			continue;

		if (UEdGraphNode* Node = Cast<UEdGraphNode>(SelectedObj))
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
	FGraphPanelSelectionSet SelectedNodes = GetSelectedNodes();
	FGraphPanelSelectionSet NodesToCopy;

	for (UObject* SelectedObj : SelectedNodes)
	{
		URuleNodeBase* Node = Cast<URuleNodeBase>(SelectedObj);
		if (Node && Node->CanDuplicateNode())
		{
			NodesToCopy.Add(Node);
			Node->PrepareForCopying();
		}
	}

	FString ExportedText;
	FEdGraphUtilities::ExportNodesToText(NodesToCopy, ExportedText);
	FPlatformApplicationMisc::ClipboardCopy(*ExportedText);
	UE_LOG(LogTemp, Warning, TEXT("%s"), *ExportedText);

	for (UObject* CopiedNode : NodesToCopy)
	{
		URuleNodeBase* Node = Cast<URuleNodeBase>(CopiedNode);
		if (Node)
			Node->PostCopyNode();
	}
}

bool FDungeonRulesToolkit::CanCopyNodes() const
{
	// If any of the nodes can be duplicated then we should allow copying
	const FGraphPanelSelectionSet SelectedNodes = GetSelectedNodes();
	for (UObject* SelectedObj : SelectedNodes)
	{
		UEdGraphNode* Node = Cast<UEdGraphNode>(SelectedObj);
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
	TSharedPtr<SGraphEditor> CurrentGraphEditor = UpdateGraphEdPtr.Pin();
	if (!CurrentGraphEditor.IsValid())
	{
		return;
	}

	// Undo/Redo support
	const FScopedTransaction Transaction(FGenericCommands::Get().Paste->GetDescription());
	UEdGraph* EdGraph = CurrentGraphEditor->GetCurrentGraph();
	UDungeonRulesGraph* RulesGraph = Cast<UDungeonRulesGraph>(EdGraph);

	EdGraph->Modify();
	if (RulesGraph)
		RulesGraph->LockUpdates();

#if false // TODO: Copied from AIGraph
	UAIGraphNode* SelectedParent = NULL;
	bool bHasMultipleNodesSelected = false;

	const FGraphPanelSelectionSet SelectedNodes = GetSelectedNodes();
	for (UObject* SelectedObj : SelectedNodes)
	{
		URuleNodeBase* Node = Cast<URuleNodeBase>(SelectedObj);
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
#endif

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
	for (UEdGraphNode* Node : PastedNodes)
	{
		if (Node)
		{
			AvgNodePosition.X += Node->NodePosX;
			AvgNodePosition.Y += Node->NodePosY;
			++AvgCount;
		}
	}

	if (AvgCount > 0)
	{
		float InvNumNodes = 1.0f / float(AvgCount);
		AvgNodePosition.X *= InvNumNodes;
		AvgNodePosition.Y *= InvNumNodes;
	}

#if false // TODO: Copied from AIGraph
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

	FixupPastedNodes(PastedNodes, NewToOldNodeMapping);
#else
	for (UEdGraphNode* PastedNode : PastedNodes)
	{
		if (!PastedNode)
			continue;

		CurrentGraphEditor->SetNodeSelection(PastedNode, true);

		PastedNode->NodePosX = (PastedNode->NodePosX - AvgNodePosition.X) + Location.X;
		PastedNode->NodePosY = (PastedNode->NodePosY - AvgNodePosition.Y) + Location.Y;

		PastedNode->SnapToGrid(SNodePanel::GetSnapGridSize());

		// Give new node a different Guid from the old one
		PastedNode->CreateNewGuid();
	}
#endif

	if (RulesGraph)
	{
#if false // TODO: Copied from AIGraph
		RulesGraph->UpdateClassData();
#endif
		RulesGraph->OnNodesPasted(TextToImport);
		RulesGraph->UnlockUpdates();
	}

	// Update UI
	CurrentGraphEditor->NotifyGraphChanged();

	UObject* GraphOwner = EdGraph->GetOuter();
	if (GraphOwner)
	{
		GraphOwner->PostEditChange();
		GraphOwner->MarkPackageDirty();
	}
}

#if false // TODO: copied from AIGraph
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

void FDungeonRulesToolkit::OnRenameNode()
{
	TSharedPtr<SGraphEditor> CurrentGraphEditor = UpdateGraphEdPtr.Pin();
	if (CurrentGraphEditor.IsValid())
	{
		const FGraphPanelSelectionSet SelectedNodes = GetSelectedNodes();
		for (FGraphPanelSelectionSet::TConstIterator NodeIt(SelectedNodes); NodeIt; ++NodeIt)
		{
			UEdGraphNode* SelectedNode = Cast<UEdGraphNode>(*NodeIt);
			if (SelectedNode != nullptr && SelectedNode->GetCanRenameNode())
			{
				//FKismetEditorUtilities::BringKismetToFocusAttentionOnObject(SelectedNode, true);
				CurrentGraphEditor->IsNodeTitleVisible(SelectedNode, /*bRequestRename*/ true);
				break;
			}
		}
	}
}

bool FDungeonRulesToolkit::CanRenameNodes() const
{
	UEdGraph* EditingGraph = UpdateGraphEdPtr.Pin()->GetCurrentGraph();
	if (!EditingGraph || !EditingGraph->bEditable)
		return false;

	const FGraphPanelSelectionSet SelectedNodes = GetSelectedNodes();
	for (const UObject* Node : SelectedNodes)
	{
		const UEdGraphNode* SelectedNode = Cast<const UEdGraphNode>(Node);
		if (SelectedNode->GetCanRenameNode())
			return true;
	}

	return false;
}

bool FDungeonRulesToolkit::OnNodeVerifyTitleCommit(const FText& NewText, UEdGraphNode* NodeBeingChanged, FText& OutErrorMessage)
{

	if (!NodeBeingChanged || !NodeBeingChanged->GetCanRenameNode())
		return false;

	// Clear off any existing error message 
	NodeBeingChanged->ErrorMsg.Empty();
	NodeBeingChanged->bHasCompilerMessage = false;

	TSharedPtr<INameValidatorInterface> NameEntryValidator = FNameValidatorFactory::MakeValidator(NodeBeingChanged);

	EValidatorResult Result = NameEntryValidator->IsValid(NewText.ToString(), true);
	if (Result == EValidatorResult::Ok)
		return true;

	else if (UpdateGraphEdPtr.IsValid())
	{
		NodeBeingChanged->bHasCompilerMessage = true;
		NodeBeingChanged->ErrorMsg = NameEntryValidator->GetErrorString(NewText.ToString(), Result);
		NodeBeingChanged->ErrorType = EMessageSeverity::Error;
	}

	return false;
}

void FDungeonRulesToolkit::OnNodeTitleCommitted(const FText& NewText, ETextCommit::Type CommitInfo, UEdGraphNode* NodeBeingChanged)
{
	if (NodeBeingChanged)
	{
		const FScopedTransaction Transaction(NSLOCTEXT("K2_RenameNode", "RenameNode", "Rename Node"));
		NodeBeingChanged->Modify();
		NodeBeingChanged->OnRenameNode(NewText.ToString());
	}
}

#undef LOCTEXT_NAMESPACE
