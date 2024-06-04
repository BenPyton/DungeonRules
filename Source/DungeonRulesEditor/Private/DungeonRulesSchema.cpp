// Copyright Epic Games, Inc. All Rights Reserved.

/*=============================================================================
	AnimationStateMachineSchema.cpp
=============================================================================*/

#include "DungeonRulesSchema.h"
#include "Layout/SlateRect.h"
#include "Animation/AnimationAsset.h"
#include "Framework/MultiBox/MultiBoxBuilder.h"
#include "ToolMenus.h"
#include "EdGraph/EdGraph.h"
#include "Kismet2/BlueprintEditorUtils.h"
#include "Settings/EditorStyleSettings.h"
#include "EdGraphNode_Comment.h"

#include "DungeonRulesGraph.h"
//#include "DungeonRulesGraphSchema.h"
#include "Nodes/RuleEntryNode.h"
#include "Nodes/RuleExitNode.h"
#include "Nodes/RuleNodeBase.h"
#include "Nodes/RuleNode.h"
#include "Nodes/RuleTransitionNode.h"
#include "Nodes/RuleConduitNode.h"
#include "Nodes/RuleAliasNode.h"
//#include "AnimGraphNode_AssetPlayerBase.h"
#include "ScopedTransaction.h"
#include "GraphEditorActions.h"

#include "EdGraphUtilities.h"
#include "Kismet2/KismetEditorUtilities.h"
#include "Framework/Commands/GenericCommands.h"

#define LOCTEXT_NAMESPACE "DungeonRulesSchema"

/////////////////////////////////////////////////////

TSharedPtr<FDungeonRulesGraphSchemaAction_NewStateNode> AddNewStateNodeAction(FGraphContextMenuBuilder& ContextMenuBuilder, const FText& Category, const FText& MenuDesc, const FText& Tooltip, const int32 Grouping = 0)
{
	TSharedPtr<FDungeonRulesGraphSchemaAction_NewStateNode> NewStateNode( new FDungeonRulesGraphSchemaAction_NewStateNode(Category, MenuDesc, Tooltip, Grouping) );
	ContextMenuBuilder.AddAction( NewStateNode );
	return NewStateNode;
}

/////////////////////////////////////////////////////
// FDungeonRulesGraphSchemaAction_NewStateNode

UEdGraphNode* FDungeonRulesGraphSchemaAction_NewStateNode::PerformAction(class UEdGraph* ParentGraph, UEdGraphPin* FromPin, const FVector2D Location, bool bSelectNewNode/* = true*/)
{
	// No action to perform if no template
	if (!NodeTemplate)
		return nullptr;

	const FScopedTransaction Transaction( NSLOCTEXT("UnrealEd", "K2_AddNode", "Add Node") );
	ParentGraph->Modify();
	if (FromPin)
	{
		FromPin->Modify();
	}

	NodeTemplate->SetFlags(RF_Transactional);

	// set outer to be the graph so it doesn't go away
	NodeTemplate->Rename(nullptr, ParentGraph, REN_NonTransactional);
	ParentGraph->AddNode(NodeTemplate, true, bSelectNewNode);

	NodeTemplate->CreateNewGuid();
	NodeTemplate->PostPlacedNewNode();
	NodeTemplate->NodePosX = static_cast<int32>(Location.X);
	NodeTemplate->NodePosY = static_cast<int32>(Location.Y);
	NodeTemplate->SnapToGrid(GetDefault<UEditorStyleSettings>()->GridSnapSize);

	NodeTemplate->AllocateDefaultPins();
	NodeTemplate->AutowireNewNode(FromPin);

	return NodeTemplate;
}

void FDungeonRulesGraphSchemaAction_NewStateNode::AddReferencedObjects( FReferenceCollector& Collector )
{
	FEdGraphSchemaAction::AddReferencedObjects( Collector );

	// These don't get saved to disk, but we want to make sure the objects don't get GC'd while the action array is around
	Collector.AddReferencedObject( NodeTemplate );
}

/////////////////////////////////////////////////////
// FDungeonRulesGraphSchemaAction_NewStateComment

UEdGraphNode* FDungeonRulesGraphSchemaAction_NewStateComment::PerformAction(class UEdGraph* ParentGraph, UEdGraphPin* FromPin, const FVector2D Location, bool bSelectNewNode/* = true*/)
{
	// Add menu item for creating comment boxes
	UEdGraphNode_Comment* CommentTemplate = NewObject<UEdGraphNode_Comment>();

	//UBlueprint* Blueprint = FBlueprintEditorUtils::FindBlueprintForGraph(ParentGraph);

	FVector2D SpawnLocation = Location;
	FSlateRect Bounds;

	TSharedPtr<SGraphEditor> GraphEditorPtr = SGraphEditor::FindGraphEditorForGraph(ParentGraph);
	//if ((Blueprint != NULL) && FKismetEditorUtilities::GetBoundsForSelectedNodes(Blueprint, Bounds, 50.0f))
	if (GraphEditorPtr.IsValid() && GraphEditorPtr->GetBoundsForSelectedNodes(/*out*/ Bounds, 50.0f))
	{
		CommentTemplate->SetBounds(Bounds);
		SpawnLocation.X = CommentTemplate->NodePosX;
		SpawnLocation.Y = CommentTemplate->NodePosY;
	}

	return FEdGraphSchemaAction_NewNode::SpawnNodeFromTemplate<UEdGraphNode_Comment>(ParentGraph, CommentTemplate, SpawnLocation, bSelectNewNode);
}

/////////////////////////////////////////////////////
// UAnimationStateMachineSchema

UDungeonRulesSchema::UDungeonRulesSchema()
	: Super()
{
}

void UDungeonRulesSchema::CreateDefaultNodesForGraph(UEdGraph& Graph) const
{
	// Create the entry/exit tunnels
	FGraphNodeCreator<URuleEntryNode> EntryNodeCreator(Graph);
	URuleEntryNode* EntryNode = EntryNodeCreator.CreateNode();
	EntryNodeCreator.Finalize();
	SetNodeMetaData(EntryNode, FNodeMetadata::DefaultGraphNode);

	if (UDungeonRulesGraph* DungeonRulesGraph = CastChecked<UDungeonRulesGraph>(&Graph))
	{
		DungeonRulesGraph->EntryNode = EntryNode;
	}

	FGraphNodeCreator<URuleExitNode> ExitNodeCreator(Graph);
	URuleExitNode* ExitNode = ExitNodeCreator.CreateNode();
	ExitNodeCreator.Finalize();
	SetNodeMetaData(ExitNode, FNodeMetadata::DefaultGraphNode);

	if (UDungeonRulesGraph* DungeonRulesGraph = CastChecked<UDungeonRulesGraph>(&Graph))
	{
		DungeonRulesGraph->ExitNode = ExitNode;
	}
}

const FPinConnectionResponse UDungeonRulesSchema::CanCreateConnection(const UEdGraphPin* PinA, const UEdGraphPin* PinB) const
{
	if (!PinA || !PinB)
		return FPinConnectionResponse(CONNECT_RESPONSE_DISALLOW, TEXT("One pin is invalid"));

	// Make sure the pins are not on the same node
	if (PinA->GetOwningNode() == PinB->GetOwningNode())
	{
		return FPinConnectionResponse(CONNECT_RESPONSE_DISALLOW, TEXT("Both are on the same node"));
	}

	// Connect entry node to a state is OK
	const bool bPinAIsEntry = PinA->GetOwningNode()->IsA(URuleEntryNode::StaticClass());
	const bool bPinBIsEntry = PinB->GetOwningNode()->IsA(URuleEntryNode::StaticClass());
	const bool bPinAIsStateNode = PinA->GetOwningNode()->IsA(URuleNodeBase::StaticClass());
	const bool bPinBIsStateNode = PinB->GetOwningNode()->IsA(URuleNodeBase::StaticClass());

	// Special case handling for entry states: Only allow creating connections starting at the entry state.
	if (bPinAIsEntry || bPinBIsEntry)
	{
		if (bPinAIsEntry && bPinBIsStateNode)
		{
			return FPinConnectionResponse(CONNECT_RESPONSE_BREAK_OTHERS_A, TEXT(""));
		}

		return FPinConnectionResponse(CONNECT_RESPONSE_DISALLOW, TEXT("Entry must connect to a state node"));
	}

	const bool bPinAIsTransition = PinA->GetOwningNode()->IsA(URuleTransitionNode::StaticClass());
	const bool bPinBIsTransition = PinB->GetOwningNode()->IsA(URuleTransitionNode::StaticClass());

	if (bPinAIsTransition && bPinBIsTransition)
	{
		return FPinConnectionResponse(CONNECT_RESPONSE_DISALLOW, TEXT("Cannot wire a transition to a transition"));
	}
	
	// Transitions are exclusive (both input and output), but states are not
	if (bPinAIsTransition)
	{
		return FPinConnectionResponse(CONNECT_RESPONSE_BREAK_OTHERS_A, TEXT(""));
	}
	else if (bPinBIsTransition)
	{
		return FPinConnectionResponse(CONNECT_RESPONSE_BREAK_OTHERS_B, TEXT(""));
	}
	else if (!bPinAIsTransition && !bPinBIsTransition)
	{
		return FPinConnectionResponse(CONNECT_RESPONSE_MAKE_WITH_CONVERSION_NODE, TEXT("Create a transition"));
	}
	else
	{
		return FPinConnectionResponse(CONNECT_RESPONSE_MAKE, TEXT(""));
	}
}

bool UDungeonRulesSchema::TryCreateConnection(UEdGraphPin* PinA, UEdGraphPin* PinB) const
{
	if (PinB->Direction == PinA->Direction)
	{
		if (URuleNodeBase* Node = Cast<URuleNodeBase>(PinB->GetOwningNode()))
		{
			if (PinA->Direction == EGPD_Input)
			{
				PinB = Node->GetOutputPin();
			}
			else
			{
				PinB = Node->GetInputPin();
			}
		}
	}

	const bool bModified = UEdGraphSchema::TryCreateConnection(PinA, PinB);

	return bModified;
}

bool UDungeonRulesSchema::CreateAutomaticConversionNodeAndConnections(UEdGraphPin* PinA, UEdGraphPin* PinB) const
{
	// Reaching here should have taken care of the pin direction.
	check(PinA->Direction != PinB->Direction);

	URuleNodeBase* NodeFrom = Cast<URuleNodeBase>((PinA->Direction == EGPD_Output) ? PinA->GetOwningNode() : PinB->GetOwningNode());
	URuleNodeBase* NodeTo = Cast<URuleNodeBase>((PinA->Direction == EGPD_Input) ? PinA->GetOwningNode() : PinB->GetOwningNode());

	if (NodeFrom && NodeTo
		&& (NodeFrom->GetOutputPin() != nullptr)
		&& (NodeTo->GetInputPin() != nullptr))
	{
		URuleTransitionNode* TransitionNode = FDungeonRulesGraphSchemaAction_NewStateNode::SpawnNodeFromTemplate<URuleTransitionNode>(NodeFrom->GetGraph(), NewObject<URuleTransitionNode>(), FVector2D(0.0f, 0.0f), false);
		TransitionNode->CreateConnections(NodeFrom, NodeTo);

		return true;
	}

	return false;
}

bool UDungeonRulesSchema::TryRelinkConnectionTarget(UEdGraphPin* SourcePin, UEdGraphPin* OldTargetPin, UEdGraphPin* NewTargetPin, const TArray<UEdGraphNode*>& InSelectedGraphNodes) const
{
	const FPinConnectionResponse Response = CanCreateConnection(SourcePin, NewTargetPin);
	if (Response.Response == ECanCreateConnectionResponse::CONNECT_RESPONSE_DISALLOW)
	{
		return false;
	}

	URuleNodeBase* OldTargetState = Cast<URuleNodeBase>(OldTargetPin->GetOwningNode());
	URuleNodeBase* NewTargetState = Cast<URuleNodeBase>(NewTargetPin->GetOwningNode());
	if (!OldTargetState || !NewTargetState)
		return false;

	// In the case we are relinking the transition starting at the entry state, the SourceState is nullptr. Special case handling.
	URuleEntryNode* EntryState = Cast<URuleEntryNode>(SourcePin->GetOwningNode());
	if (EntryState)
	{
		// Add the new incoming transition to the new target state
		TryCreateConnection(SourcePin, NewTargetPin);

		return true;
	}

	// Collect all transition nodes starting at the source state, filter them by the transitions and perform the actual relink operation.
	const TArray<URuleTransitionNode*> TransitionNodes = URuleTransitionNode::GetListTransitionNodesToRelink(SourcePin, OldTargetPin, InSelectedGraphNodes);
	for (URuleTransitionNode* TransitionNode : TransitionNodes)
	{
		TransitionNode->RelinkHead(NewTargetState);
	}

	// In case one or more transitions got relinked, inform the blueprint about the changes
#if WITH_EDITOR
	if (!TransitionNodes.IsEmpty())
	{
		SourcePin->GetOwningNode()->PinConnectionListChanged(SourcePin);
		OldTargetPin->GetOwningNode()->PinConnectionListChanged(OldTargetPin);
		NewTargetPin->GetOwningNode()->PinConnectionListChanged(NewTargetPin);
	}
#endif//#if WITH_EDITOR

	return true;
}

bool UDungeonRulesSchema::IsConnectionRelinkingAllowed(UEdGraphPin* InPin) const
{
	if (InPin && InPin->GetOwningNode())
	{
		URuleNodeBase* StateNode = Cast<URuleNodeBase>(InPin->GetOwningNode());
		URuleTransitionNode* TransitionNode = Cast<URuleTransitionNode>(InPin->GetOwningNode());
		URuleEntryNode* EntryNode = Cast<URuleEntryNode>(InPin->GetOwningNode());
		if (StateNode || TransitionNode || EntryNode)
		{
			return true;
		}
	}

	return false;
}

const FPinConnectionResponse UDungeonRulesSchema::CanRelinkConnectionToPin(const UEdGraphPin* OldSourcePin, const UEdGraphPin* TargetPinCandidate) const
{
	FPinConnectionResponse Response = CanCreateConnection(OldSourcePin, TargetPinCandidate);
	if (Response.Response != CONNECT_RESPONSE_DISALLOW)
	{
		Response.Message = FText::FromString("Relink transition");
	}

	return Response;
}

void UDungeonRulesSchema::GetGraphContextActions(FGraphContextMenuBuilder& ContextMenuBuilder) const
{
	// Add state node
	{
		TSharedPtr<FDungeonRulesGraphSchemaAction_NewStateNode> Action = AddNewStateNodeAction(ContextMenuBuilder, FText::GetEmpty(), LOCTEXT("AddState", "Add State"), LOCTEXT("AddStateTooltip", "A new state"));
		Action->NodeTemplate = NewObject<URuleNode>(ContextMenuBuilder.OwnerOfTemporaries);
	}

#if false // TODO: alias and conduit nodes
	// Add state alias node
	{
		TSharedPtr<FDungeonRulesGraphSchemaAction_NewStateNode> Action = AddNewStateNodeAction(ContextMenuBuilder, FText::GetEmpty(), LOCTEXT("AddStateAlias", "Add State Alias"), LOCTEXT("AddStateAliasTooltip", "A new state alias"));
		Action->NodeTemplate = NewObject<URuleAliasNode>(ContextMenuBuilder.OwnerOfTemporaries);
	}

	// Add conduit node
	{
		TSharedPtr<FDungeonRulesGraphSchemaAction_NewStateNode> Action = AddNewStateNodeAction(ContextMenuBuilder, FText::GetEmpty(), LOCTEXT("AddConduit", "Add Conduit"), LOCTEXT("AddConduitTooltip", "A new conduit state"));
		Action->NodeTemplate = NewObject<URuleConduitNode>(ContextMenuBuilder.OwnerOfTemporaries);
	}
#endif

	// Entry point (only if doesn't already exist)
	{
		bool bHasEntry = false;
		for (auto NodeIt = ContextMenuBuilder.CurrentGraph->Nodes.CreateConstIterator(); NodeIt; ++NodeIt)
		{
			UEdGraphNode* Node = *NodeIt;
			if (const URuleEntryNode* StateNode = Cast<URuleEntryNode>(Node))
			{
				bHasEntry = true;
				break;
			}
		}

		if (!bHasEntry)
		{
			TSharedPtr<FDungeonRulesGraphSchemaAction_NewStateNode> Action = AddNewStateNodeAction(ContextMenuBuilder, FText::GetEmpty(), LOCTEXT("AddEntryPoint", "Add Entry Point"), LOCTEXT("AddEntryPointTooltip", "Define State Machine's Entry Point"));
			Action->NodeTemplate = NewObject<URuleEntryNode>(ContextMenuBuilder.OwnerOfTemporaries);
		}
	}

	// Add Comment
	if (!ContextMenuBuilder.FromPin)
	{
		// TODO: Get number of selected nodes.
		const bool bIsManyNodesSelected = false;
		const FText MenuDescription = bIsManyNodesSelected ? LOCTEXT("CreateCommentSelection", "Create Comment from Selection") : LOCTEXT("AddComment", "Add Comment");
		const FText ToolTip = LOCTEXT("CreateCommentSelectionTooltip", "Create a resizeable comment box around selected nodes.");

		TSharedPtr<FDungeonRulesGraphSchemaAction_NewStateComment> NewComment( new FDungeonRulesGraphSchemaAction_NewStateComment(FText::GetEmpty(), MenuDescription, ToolTip, 0) );
		ContextMenuBuilder.AddAction( NewComment );
	}
}

EGraphType UDungeonRulesSchema::GetGraphType(const UEdGraph* TestEdGraph) const
{
	return GT_StateMachine;
}

void UDungeonRulesSchema::GetContextMenuActions(UToolMenu* Menu, UGraphNodeContextMenuContext* Context) const
{
	check(Context && Context->Graph);
	if (Context->Node)
	{
		FToolMenuSection& Section = Menu->AddSection("DungeonRulesMachineNodeActions", LOCTEXT("NodeActionsMenuHeader", "Node Actions"));
		if (!Context->bIsDebugging)
		{
			// Node contextual actions
			Section.AddMenuEntry(FGenericCommands::Get().Delete);
			Section.AddMenuEntry(FGenericCommands::Get().Cut);
			Section.AddMenuEntry(FGenericCommands::Get().Copy);
			Section.AddMenuEntry(FGenericCommands::Get().Duplicate);
			Section.AddMenuEntry(FGraphEditorCommands::Get().ReconstructNodes);
			Section.AddMenuEntry(FGraphEditorCommands::Get().BreakNodeLinks);
			if(Context->Node->bCanRenameNode)
			{
				Section.AddMenuEntry(FGenericCommands::Get().Rename);
			}
		}
	}

	Super::GetContextMenuActions(Menu, Context);
}

FLinearColor UDungeonRulesSchema::GetPinTypeColor(const FEdGraphPinType& PinType) const
{
	if (PinType.PinCategory == TEXT("Transition"))
	{
		return FLinearColor::White;
	}

	return GetDefault<UEdGraphSchema_K2>()->GetPinTypeColor(PinType);
}

void UDungeonRulesSchema::GetGraphDisplayInformation(const UEdGraph& Graph, /*out*/ FGraphDisplayInfo& DisplayInfo) const
{
	DisplayInfo.PlainName = FText::FromString( Graph.GetName() );
	DisplayInfo.DisplayName = DisplayInfo.PlainName;
	DisplayInfo.Tooltip = LOCTEXT("GraphTooltip_StateMachineSchema", "Graph used to transition between different states each with separate animation graphs");
}

#if false // TODO: asset drop on graph
void UDungeonRulesSchema::DroppedAssetsOnGraph(const TArray<FAssetData>& Assets, const FVector2D& GraphPosition, UEdGraph* Graph) const
{
	UAnimationAsset* Asset = FAssetData::GetFirstAsset<UAnimationAsset>(Assets);
	if(Asset != NULL && GetNodeClassForAsset(Asset->GetClass()))
	{
		// Spawn new state
		URuleNode* NewStateNode = FDungeonRulesGraphSchemaAction_NewStateNode::SpawnNodeFromTemplate<URuleNode>(Graph, NewObject<URuleNode>(), GraphPosition);

		// Try to name the state close to the asset
		FEdGraphUtilities::RenameGraphToNameOrCloseToName(NewStateNode->BoundGraph, Asset->GetName());

		// Change the current graph context to the inner graph, so that the rest of the drag drop happens inside it
		FVector2D NewGraphPosition = FVector2D(-300.0f, 0.0f);
		UAnimationGraphSchema::SpawnNodeFromAsset(Asset, NewGraphPosition, NewStateNode->BoundGraph, NewStateNode->GetPoseSinkPinInsideState());
	}
}
#endif

#if false // TODO: asset drop on graph
void UDungeonRulesSchema::DroppedAssetsOnNode(const TArray<FAssetData>& Assets, const FVector2D& GraphPosition, UEdGraphNode* Node) const
{
	UAnimationAsset* Asset = FAssetData::GetFirstAsset<UAnimationAsset>(Assets);
	URuleNode* StateNodeUnderCursor = Cast<URuleNode>(Node);
	if(Asset != NULL && StateNodeUnderCursor != NULL)
	{
		// Dropped onto a state machine state; try some user friendly resposnes
		if (UEdGraphPin* PosePin = StateNodeUnderCursor->GetPoseSinkPinInsideState())
		{
			if (PosePin->LinkedTo.Num() > 0)
			{
				//@TODO: A2REMOVAL: This doesn't do anything
				/*
				// Try dropping in onto the node attached to the sink inside the state
				check(PosePin->LinkedTo[0] != NULL);
				UA2Node* A2Node = Cast<UA2Node>(PosePin->LinkedTo[0]->GetOwningNode());
				if(A2Node != NULL)
				{
					UAnimationGraphSchema::UpdateNodeWithAsset(A2Node, Asset);
				}
				*/
			}
			else
			{
				// Try dropping onto the pose pin, since nothing is currently connected
				const FVector2D NewGraphPosition(-300.0f, 0.0f);
				UAnimationGraphSchema::SpawnNodeFromAsset(Asset, NewGraphPosition, StateNodeUnderCursor->BoundGraph, PosePin);
			}
		}
	}
}

void UDungeonRulesSchema::DroppedAssetsOnPin(const TArray<FAssetData>& Assets, const FVector2D& GraphPosition, UEdGraphPin* Pin) const
{
	// unused for state machines?
}

void UDungeonRulesSchema::GetAssetsNodeHoverMessage(const TArray<FAssetData>& Assets, const UEdGraphNode* HoverNode, FString& OutTooltipText, bool& OutOkIcon) const
{ 
	UAnimationAsset* Asset = FAssetData::GetFirstAsset<UAnimationAsset>(Assets);
	if ((Asset == NULL) || (HoverNode == NULL))
	{
		OutTooltipText = TEXT("");
		OutOkIcon = false;
		return;
	}

	const URuleNode* StateNodeUnderCursor = Cast<const URuleNode>(HoverNode);
	if (StateNodeUnderCursor != NULL)
	{
		OutOkIcon = true;
		OutTooltipText = FString::Printf(TEXT("Change node to play %s"), *(Asset->GetName()));
	}
	else
	{
		OutTooltipText = TEXT("");
		OutOkIcon = false;
	}
}

void UDungeonRulesSchema::GetAssetsPinHoverMessage(const TArray<FAssetData>& Assets, const UEdGraphPin* HoverPin, FString& OutTooltipText, bool& OutOkIcon) const
{ 
	// unused for state machines?

	OutTooltipText = TEXT("");
	OutOkIcon = false;
}
#endif

void UDungeonRulesSchema::BreakNodeLinks(UEdGraphNode& TargetNode) const
{
	const FScopedTransaction Transaction(NSLOCTEXT("UnrealEd", "GraphEd_BreakNodeLinks", "Break Node Links"));
	Super::BreakNodeLinks(TargetNode);
}

void UDungeonRulesSchema::BreakPinLinks(UEdGraphPin& TargetPin, bool bSendsNodeNotification) const
{
	const FScopedTransaction Transaction(NSLOCTEXT("UnrealEd", "GraphEd_BreakPinLinks", "Break Pin Links"));
	Super::BreakPinLinks(TargetPin, bSendsNodeNotification);
}

void UDungeonRulesSchema::BreakSinglePinLink(UEdGraphPin* SourcePin, UEdGraphPin* TargetPin) const
{
	const FScopedTransaction Transaction(NSLOCTEXT("UnrealEd", "GraphEd_BreakSinglePinLink", "Break Pin Link"));
	Super::BreakSinglePinLink(SourcePin, TargetPin);
}

#undef LOCTEXT_NAMESPACE
