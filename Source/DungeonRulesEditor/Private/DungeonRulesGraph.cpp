// Copyright Epic Games, Inc. All Rights Reserved.

#include "DungeonRulesGraph.h"
#include "Nodes/RuleNodeBase.h"

#define LOCTEXT_NAMESPACE "DungeonRulesGraph"

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

#if false
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

#if false
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
}

void UDungeonRulesGraph::OnCreated()
{
	MarkVersion();
}

void UDungeonRulesGraph::OnLoaded()
{
	UpdateDeprecatedClasses();
	UpdateUnknownNodeClasses();
}

void UDungeonRulesGraph::Initialize()
{
	UpdateVersion();
}

void UDungeonRulesGraph::UpdateVersion()
{
	if (GraphVersion == 1)
	{
		return;
	}

	MarkVersion();
	Modify();
}

void UDungeonRulesGraph::MarkVersion()
{
	GraphVersion = 1;
}

bool UDungeonRulesGraph::UpdateUnknownNodeClasses()
{
	bool bUpdated = false;
	/*for (int32 NodeIdx = 0; NodeIdx < Nodes.Num(); NodeIdx++)
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
	}*/

	return bUpdated;
}

void UpdateAIGraphNodeErrorMessage(URuleNodeBase& Node)
{
	// Broke out setting error message in to own function so it can be reused when iterating nodes collection.
	/*if (Node.NodeInstance)
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
	}*/
}

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

void UDungeonRulesGraph::Serialize(FArchive& Ar)
{
	// Overridden to flags up errors in the behavior tree while cooking.
	Super::Serialize(Ar);

	if (Ar.IsSaving() || Ar.IsCooking())
	{
		// Logging of errors happens in UpdateDeprecatedClasses
		UpdateDeprecatedClasses();
	}
}

void UDungeonRulesGraph::UpdateClassData()
{
	for (int32 Idx = 0; Idx < Nodes.Num(); Idx++)
	{
		URuleNodeBase* Node = Cast<URuleNodeBase>(Nodes[Idx]);
		if (Node)
		{
#if false
			Node->UpdateNodeClassData();
#endif
		}
	}
}

void UDungeonRulesGraph::CollectAllNodeInstances(TSet<UObject*>& NodeInstances)
{
	for (int32 Idx = 0; Idx < Nodes.Num(); Idx++)
	{
		URuleNodeBase* MyNode = Cast<URuleNodeBase>(Nodes[Idx]);
		if (MyNode)
		{
#if false
			NodeInstances.Add(MyNode->NodeInstance);
#endif
		}
	}
}

bool UDungeonRulesGraph::CanRemoveNestedObject(UObject* TestObject) const
{
	return !TestObject->IsA(UEdGraphNode::StaticClass()) &&
		!TestObject->IsA(UEdGraph::StaticClass()) &&
		!TestObject->IsA(UEdGraphSchema::StaticClass());
}

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
	UpdateAsset();
}

#undef LOCTEXT_NAMESPACE
