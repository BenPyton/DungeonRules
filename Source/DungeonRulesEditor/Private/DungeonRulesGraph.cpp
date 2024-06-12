// Copyright (c) 2024 Benoit Pelletier
// SPDX-License-Identifier: BSL-1.0
// Distributed under the Boost Software License, Version 1.0. 
// (See accompanying file LICENSE or copy at https://www.boost.org/LICENSE_1_0.txt)

#include "DungeonRulesGraph.h"
#include "Nodes/RuleAliasNode.h"
#include "Nodes/RuleConduitNode.h"
#include "Nodes/RuleEntryNode.h"
#include "Nodes/RuleNode.h"
#include "Nodes/RuleTransitionNode.h"
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

void UDungeonRulesGraph::UpdateAsset(int32 UpdateFlags)
{
	if (bLockUpdates)
	{
		return;
	}

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

void UDungeonRulesGraph::Serialize(FArchive& Ar)
{
	// Overridden to flags up errors in the behavior tree while cooking.
	Super::Serialize(Ar);
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
	//UpdateAsset();
}

#undef LOCTEXT_NAMESPACE
