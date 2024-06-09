// Copyright Epic Games, Inc. All Rights Reserved.

#include "DungeonRulesGraph.h"
#include "Nodes/RuleNodeBase.h"
#include "Nodes/RuleNode.h"
#include "Nodes/RuleTransitionNode.h"
#include "Nodes/RuleEntryNode.h"
#include "Nodes/RuleAliasNode.h"
#include "Nodes/RuleConduitNode.h"
#include "DungeonRules.h"
#include "DUngeonRulesEdLog.h"

#define LOCTEXT_NAMESPACE "DungeonRulesGraph"

// Manage the version of the graph.
// When modifying the structure of the graph, a new version should be added.
// (don't forget to update the Latest value!!!)
// (also, don't forget to update the UpdateVersion() to upgrade the graph to the latest version!!!)
namespace DungeonRulesVersion
{
	const int32 Initial = 0;
	const int32 Latest = Initial;
}

UDungeonRulesGraph::UDungeonRulesGraph()
	: Super()
{
	bAllowDeletion = false;
	bAllowRenaming = true;
	bLockUpdates = false;
}

void UDungeonRulesGraph::OnSave()
{
	UpdateAsset();
}

#if false // TODO: should be removed
void UDungeonRulesGraph::RefreshNodes()
{
	for (UEdGraphNode* Node : Nodes)
	{
		if (URuleNodeBase* RuleNode = Cast<URuleNodeBase>(Node))
			RuleNode->UpdateVisualNode();
	}
}
#endif

void UDungeonRulesGraph::UpdateAsset(int32 UpdateFlags)
{
	if (bLockUpdates)
	{
		return;
	}

#if false // TODO: cherry pick what could be useful here too
	ActionTreeEd_LogInfo("Starting to link all asset nodes from the editor graph links.");

	// initial cleanup & root node search
	UATGraphNode_Root* RootNode = nullptr;
	for (int32 Index = 0; Index < Nodes.Num(); ++Index)
	{
		UATGraphNode* Node = Cast<UATGraphNode>(Nodes[Index]);

		if (Node == nullptr)
		{
			// ignore non-AT nodes.
			continue;
		}

		// parent chain
		Node->ParentNode = nullptr;
		for (int32 Idx = 0; Idx < Node->SubNodes.Num(); Idx++)
		{
			Node->SubNodes[Idx]->ParentNode = Node;
		}

		// prepare node instance
		UATNode* NodeInstance = Cast<UATNode>(Node->NodeInstance);
		if (NodeInstance)
		{
			// mark all nodes as disconnected first, path from root will replace it with valid values later
			NodeInstance->SetParent(nullptr);
		}

		// cache root
		if (RootNode == nullptr)
		{
			RootNode = Cast<UATGraphNode_Root>(Nodes[Index]);
		}

	} // end for loop on all graph nodes

	// we can't look at pins until pin references have been fixed up post undo:
	UEdGraphPin::ResolveAllPinReferences();

	UATGraphNode* Node = nullptr;
	if (RootNode && RootNode->Pins.Num() > 0 && RootNode->Pins[0]->LinkedTo.Num() > 0)
	{
		Node = Cast<UATGraphNode>(RootNode->Pins[0]->LinkedTo[0]->GetOwningNode());
	}

	CreateATFromGraph(Node);
#endif

	DungeonEd_LogInfo("Create DungeonRules asset from graph.");
	UDungeonRules* DungeonRulesAsset = Cast<UDungeonRules>(GetOuter());
	check(DungeonRulesAsset);

	// Clear previous data in the asset
	DungeonRulesAsset->Clear();

	// Add rule instances to the asset
	for (const UEdGraphNode* Node : Nodes)
	{
		const URuleNode* RuleNode = Cast<URuleNode>(Node);
		if (!RuleNode)
			continue;

		UDungeonRule* Rule = RuleNode->GetNodeInstance<UDungeonRule>();
		if (!Rule)
			continue;

		if (Rule->GetOuter() != DungeonRulesAsset)
		{
			DungeonEd_LogWarning("Rule %s has not the asset %s as outer!", *Rule->RuleName, *DungeonRulesAsset->GetName());
		}

		DungeonRulesAsset->AddRule(Rule);
	}

	// Add conduits to the asset
	for (const UEdGraphNode* Node : Nodes)
	{
		const URuleConduitNode* ConduitNode = Cast<URuleConduitNode>(Node);
		if (!ConduitNode)
			continue;

		URuleConduit* Conduit = ConduitNode->GetNodeInstance<URuleConduit>();
		if (!Conduit)
			continue;

		if (Conduit->GetOuter() != DungeonRulesAsset)
		{
			DungeonEd_LogWarning("Conduit %s has not the asset %s as outer!", *Conduit->GetName(), *DungeonRulesAsset->GetName());
		}

		DungeonRulesAsset->AddConduit(Conduit);
	}

	// Set the first rule
	URuleNode* FirstNode = Cast<URuleNode>(EntryNode->GetOutputNode());
	UDungeonRule* FirstRule = (FirstNode) ? FirstNode->GetNodeInstance<UDungeonRule>() : nullptr;
	DungeonRulesAsset->SetFirstRule(FirstRule);

	// Add transition instances to the asset + in the rule instances too
	for (const UEdGraphNode* Node : Nodes)
	{
		const URuleTransitionNode* TransitionNode = Cast<URuleTransitionNode>(Node);
		if (!TransitionNode)
			continue;

		UDungeonRuleTransition* Transition = TransitionNode->GetNodeInstance<UDungeonRuleTransition>();
		if (!Transition)
			continue;

		if (Transition->GetOuter() != DungeonRulesAsset)
		{
			DungeonEd_LogWarning("Transition %s has not the asset %s as outer!", *Transition->GetName(), *DungeonRulesAsset->GetName());
		}

		DungeonRulesAsset->AddTransition(Transition);

		// Add the transition into the list of the previous rule
		if (const URuleNodeBase* PrevRuleNode = TransitionNode->GetPreviousState())
		{
			if (UDungeonRule* Rule = PrevRuleNode->GetNodeInstance<UDungeonRule>())
				Rule->AddTransition(Transition);
			else if (URuleConduit* Conduit = PrevRuleNode->GetNodeInstance<URuleConduit>())
				Conduit->AddTransition(Transition);
			else if (const URuleAliasNode* AliasRuleNode = Cast<URuleAliasNode>(PrevRuleNode))
			{
				if (AliasRuleNode->bGlobalAlias)
				{
					DungeonRulesAsset->AddGlobalTransition(Transition);
				}
				else
				{
					for (const auto& AliasedState : AliasRuleNode->GetAliasedStates())
					{
						if (UDungeonRule* AliasedRule = AliasedState->GetNodeInstance<UDungeonRule>())
							AliasedRule->AddTransition(Transition);
					}
				}
			}
		}

		// Set the next rule of the transition instance
		Transition->NextRule = nullptr;
		if (URuleNodeBase* NextRuleNode = TransitionNode->GetNextState())
		{
			Transition->NextRule = NextRuleNode->GetNodeInstance();
		}
	}
}

void UDungeonRulesGraph::OnCreated()
{
	MarkVersion();
}

#if false // TODO: Copied from AIGraph
void UDungeonRulesGraph::OnLoaded()
{
	UpdateDeprecatedClasses();
	UpdateUnknownNodeClasses();
}
#endif

void UDungeonRulesGraph::Initialize()
{
	UpdateVersion();
}

void UDungeonRulesGraph::UpdateVersion()
{
	if (GraphVersion == DungeonRulesVersion::Latest)
		return;

	/** Here put the functions to upgrade the graph to the latest version. **/

	MarkVersion();
	Modify();
}

void UDungeonRulesGraph::MarkVersion()
{
	GraphVersion = DungeonRulesVersion::Latest;
}

#if false // TODO: Copied from AIGraph
bool UDungeonRulesGraph::UpdateUnknownNodeClasses()
{
	bool bUpdated = false;
	for (int32 NodeIdx = 0; NodeIdx < Nodes.Num(); NodeIdx++)
	{
		URuleNodeBase* MyNode = Cast<URuleNodeBase>(Nodes[NodeIdx]);
		if (MyNode)
		{
			const bool bUpdatedNode = MyNode->RefreshNodeClass();
			bUpdated = bUpdated || bUpdatedNode;

			for (int32 SubNodeIdx = 0; SubNodeIdx < MyNode->SubNodes.Num(); SubNodeIdx++)
			{
				if (MyNode->SubNodes[SubNodeIdx])
				{
					const bool bUpdatedSubNode = MyNode->SubNodes[SubNodeIdx]->RefreshNodeClass();
					bUpdated = bUpdated || bUpdatedSubNode;
				}
			}
		}
	}

	return bUpdated;
}
#endif

#if false // TODO: Copied from AIGraph
void UpdateAIGraphNodeErrorMessage(URuleNodeBase& Node)
{
	// Broke out setting error message in to own function so it can be reused when iterating nodes collection.
	if (Node.NodeInstance)
	{
		Node.ErrorMessage = FGraphNodeClassHelper::GetDeprecationMessage(Node.NodeInstance->GetClass());
	}
	else
	{
		// Null instance. Do we have any meaningful class data?
		FString StoredClassName = Node.ClassData.GetClassName();
		StoredClassName.RemoveFromEnd(TEXT("_C"));

		if (!StoredClassName.IsEmpty())
		{
			// There is class data here but the instance was not be created.
			static const FString IsMissingClassMessage(" class missing. Referenced by ");
			Node.ErrorMessage = StoredClassName + IsMissingClassMessage + Node.GetFullName();
		}
	}

	if (Node.HasErrors())
	{
		UE_LOG(LogAIGraph, Error, TEXT("%s"), *Node.ErrorMessage);
	}
}
#endif

#if false // TODO: Copied from AIGraph
void UDungeonRulesGraph::UpdateDeprecatedClasses()
{
	// This function sets error messages and logs errors about nodes.

	for (int32 Idx = 0, IdxNum = Nodes.Num(); Idx < IdxNum; ++Idx)
	{
		URuleNodeBase* Node = Cast<URuleNodeBase>(Nodes[Idx]);
		if (Node != nullptr)
		{
			UpdateAIGraphNodeErrorMessage(*Node);

			/*for (int32 SubIdx = 0, SubIdxNum = Node->SubNodes.Num(); SubIdx < SubIdxNum; ++SubIdx)
			{
				if (Node->SubNodes[SubIdx] != nullptr)
				{
					UpdateAIGraphNodeErrorMessage(*Node->SubNodes[SubIdx]);
				}
			}*/
		}
	}
}
#endif

void UDungeonRulesGraph::Serialize(FArchive& Ar)
{
	// Overridden to flags up errors in the behavior tree while cooking.
	Super::Serialize(Ar);

#if false // TODO: Copied from AIGraph
	if (Ar.IsSaving() || Ar.IsCooking())
	{
		// Logging of errors happens in UpdateDeprecatedClasses
		UpdateDeprecatedClasses();
	}
#endif
}

#if false // TODO: Copied from AIGraph
void UDungeonRulesGraph::UpdateClassData()
{
	for (int32 Idx = 0; Idx < Nodes.Num(); Idx++)
	{
		URuleNodeBase* Node = Cast<URuleNodeBase>(Nodes[Idx]);
		if (Node)
		{
			Node->UpdateNodeClassData();
		}
	}
}
#endif

#if false // TODO: Copied from AIGraph
void UDungeonRulesGraph::CollectAllNodeInstances(TSet<UObject*>& NodeInstances)
{
	for (int32 Idx = 0; Idx < Nodes.Num(); Idx++)
	{
		URuleNodeBase* MyNode = Cast<URuleNodeBase>(Nodes[Idx]);
		if (MyNode)
		{
			NodeInstances.Add(MyNode->NodeInstance);
		}
	}
}
#endif

#if false // TODO: Copied from AIGraph
bool UDungeonRulesGraph::CanRemoveNestedObject(UObject* TestObject) const
{
	return !TestObject->IsA(UEdGraphNode::StaticClass()) &&
		!TestObject->IsA(UEdGraph::StaticClass()) &&
		!TestObject->IsA(UEdGraphSchema::StaticClass());
}
#endif

#if false // TODO: Copied from AIGraph
void UDungeonRulesGraph::RemoveOrphanedNodes()
{
	TSet<UObject*> NodeInstances;
	CollectAllNodeInstances(NodeInstances);

	NodeInstances.Remove(nullptr);

	// Obtain a list of all nodes actually in the asset and discard unused nodes
	TArray<UObject*> AllInners;
	const bool bIncludeNestedObjects = false;
	GetObjectsWithOuter(GetOuter(), AllInners, bIncludeNestedObjects);
	for (auto InnerIt = AllInners.CreateConstIterator(); InnerIt; ++InnerIt)
	{
		UObject* TestObject = *InnerIt;
		if (!NodeInstances.Contains(TestObject) && CanRemoveNestedObject(TestObject))
		{
			OnNodeInstanceRemoved(TestObject);

			TestObject->SetFlags(RF_Transient);
			TestObject->Rename(NULL, GetTransientPackage(), REN_DontCreateRedirectors | REN_NonTransactional | REN_ForceNoResetLoaders);
		}
	}
}
#endif

void UDungeonRulesGraph::OnNodeInstanceRemoved(UObject* NodeInstance)
{
	// empty in base class
}

void UDungeonRulesGraph::OnNodesPasted(const FString& ImportStr)
{
	// empty in base class
}

UEdGraphPin* UDungeonRulesGraph::FindGraphNodePin(UEdGraphNode* Node, EEdGraphPinDirection Dir)
{
	UEdGraphPin* Pin = nullptr;
	for (int32 Idx = 0; Idx < Node->Pins.Num(); Idx++)
	{
		if (Node->Pins[Idx]->Direction == Dir)
		{
			Pin = Node->Pins[Idx];
			break;
		}
	}

	return Pin;
}

bool UDungeonRulesGraph::IsLocked() const
{
	return bLockUpdates;
}

void UDungeonRulesGraph::LockUpdates()
{
	bLockUpdates = true;
}

void UDungeonRulesGraph::UnlockUpdates()
{
	bLockUpdates = false;
	//UpdateAsset();
}

#undef LOCTEXT_NAMESPACE
