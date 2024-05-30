// Copyright Epic Games, Inc. All Rights Reserved.

/*=============================================================================
	DungeonRuleNodeBase.cpp
=============================================================================*/

#include "RuleNodeBase.h"
#include "UObject/FrameworkObjectVersion.h"
#include "Animation/AnimBlueprint.h"
#include "Animation/AnimClassInterface.h"
#include "Kismet2/BlueprintEditorUtils.h"
#include "DungeonRulesGraph.h"
#include "DungeonRulesSchema.h"
#include "Kismet2/Kismet2NameValidators.h"
#include "Kismet2/KismetEditorUtilities.h"
#include "RuleTransitionNode.h"
/////////////////////////////////////////////////////
// FDungeonRuleNodeNameValidator

class FDungeonRuleNodeNameValidator : public FStringSetNameValidator
{
public:
	FDungeonRuleNodeNameValidator(const URuleNodeBase* InStateNode)
		: FStringSetNameValidator(FString())
	{
		TArray<URuleNodeBase*> Nodes;
		UDungeonRulesGraph* StateMachine = CastChecked<UDungeonRulesGraph>(InStateNode->GetOuter());

		StateMachine->GetNodesOfClass<URuleNodeBase>(Nodes);
		for (auto NodeIt = Nodes.CreateIterator(); NodeIt; ++NodeIt)
		{
			auto Node = *NodeIt;
			if (Node != InStateNode)
			{
				Names.Add(Node->GetStateName());
			}
		}

#if false
		// Include the name of animation layers
		UBlueprint* Blueprint = FBlueprintEditorUtils::FindBlueprintForGraphChecked(StateMachine);
		
		if (Blueprint)
		{
			UClass* TargetClass = *Blueprint->SkeletonGeneratedClass;
			if (TargetClass)
			{
				IAnimClassInterface* AnimClassInterface = IAnimClassInterface::GetFromClass(TargetClass);
				for (const FAnimBlueprintFunction& AnimBlueprintFunction : AnimClassInterface->GetAnimBlueprintFunctions())
				{
					if (AnimBlueprintFunction.Name != UEdGraphSchema_K2::GN_AnimGraph)
					{
						Names.Add(AnimBlueprintFunction.Name.ToString());
					}
				}
			}
		}
#endif
	}
};

/////////////////////////////////////////////////////
// UDungeonRuleNodeBase

URuleNodeBase::URuleNodeBase()
	: Super()
{
}

void URuleNodeBase::PostPasteNode()
{
	Super::PostPasteNode();

#if false
	for(UEdGraph* SubGraph : GetSubGraphs())
	{
		if(SubGraph)
		{
			// Add the new graph as a child of our parent graph
			UEdGraph* ParentGraph = GetGraph();

			if(ParentGraph->SubGraphs.Find(SubGraph) == INDEX_NONE)
			{
				ParentGraph->SubGraphs.Add(SubGraph);
			}

			//@TODO: CONDUIT: Merge conflict - May no longer be necessary due to other changes?
	//		FBlueprintEditorUtils::RenameGraphWithSuggestion(SubGraph, NameValidator, GetDesiredNewNodeName());
			//@ENDTODO

			// Restore transactional flag that is lost during copy/paste process
			SubGraph->SetFlags(RF_Transactional);

			UBlueprint* Blueprint = FBlueprintEditorUtils::FindBlueprintForGraphChecked(ParentGraph);
			FBlueprintEditorUtils::MarkBlueprintAsStructurallyModified(Blueprint);
		}
	}
#endif
}

#if false
UObject* URuleNodeBase::GetJumpTargetForDoubleClick() const
{
	TArray<UEdGraph*> SubGraphs = GetSubGraphs();
	check(SubGraphs.Num() > 0);
	return SubGraphs[0];
}

bool URuleNodeBase::CanJumpToDefinition() const
{
	return GetJumpTargetForDoubleClick() != nullptr;
}

void URuleNodeBase::JumpToDefinition() const
{
	if (UObject* HyperlinkTarget = GetJumpTargetForDoubleClick())
	{
		FKismetEditorUtilities::BringKismetToFocusAttentionOnObject(HyperlinkTarget);
	}
}
#endif

bool URuleNodeBase::CanCreateUnderSpecifiedSchema(const UEdGraphSchema* Schema) const
{
	return Schema->IsA(UDungeonRulesSchema::StaticClass());
}

#if false
void URuleNodeBase::OnRenameNode(const FString& NewName)
{
	TArray<UEdGraph*> SubGraphs = GetSubGraphs();
	check(SubGraphs.Num() > 0);
	FBlueprintEditorUtils::RenameGraph(SubGraphs[0], NewName);
}
#endif

TSharedPtr<class INameValidatorInterface> URuleNodeBase::MakeNameValidator() const
{
	//return MakeShareable(new FDungeonRuleNodeNameValidator(this));
	return NULL;
}

#if false
FString URuleNodeBase::GetDocumentationLink() const
{
	return TEXT("Shared/GraphNodes/AnimationStateMachine");
}
#endif

void URuleNodeBase::Serialize(FArchive& Ar)
{
	Super::Serialize(Ar);
	Ar.UsingCustomVersion(FFrameworkObjectVersion::GUID);
}

void URuleNodeBase::PostLoad()
{
	Super::PostLoad();

	const int32 CustomVersion = GetLinkerCustomVersion(FFrameworkObjectVersion::GUID);

	if(CustomVersion < FFrameworkObjectVersion::FixNonTransactionalPins)
	{
		int32 BrokenPinCount = 0;
		for(UEdGraphPin_Deprecated* Pin : DeprecatedPins)
		{
			if(!Pin->HasAnyFlags(RF_Transactional))
			{
				++BrokenPinCount;
				Pin->SetFlags(Pin->GetFlags() | RF_Transactional);
			}
		}

		if(BrokenPinCount > 0)
		{
			UE_LOG(LogAnimation, Log, TEXT("Fixed %d non-transactional pins in %s"), BrokenPinCount, *GetName());
		}
	}
}

void URuleNodeBase::GetTransitionList(TArray<URuleTransitionNode*>& OutTransitions, bool bWantSortedList /*= false*/) const
{
	// Normal transitions
	for (int32 LinkIndex = 0; LinkIndex < Pins[1]->LinkedTo.Num(); ++LinkIndex)
	{
		UEdGraphNode* TargetNode = Pins[1]->LinkedTo[LinkIndex]->GetOwningNode();
		if (URuleTransitionNode* Transition = Cast<URuleTransitionNode>(TargetNode))
		{
			OutTransitions.Add(Transition);
		}
	}

#if false
	// Bidirectional transitions where we are the 'backwards' link.
	// Conduits and other states types that don't support bidirectional transitions should hide it from the details panel.
	for (int32 LinkIndex = 0; LinkIndex < Pins[0]->LinkedTo.Num(); ++LinkIndex)
	{
		UEdGraphNode* TargetNode = Pins[0]->LinkedTo[LinkIndex]->GetOwningNode();
		if (URuleTransitionNode* Transition = Cast<URuleTransitionNode>(TargetNode))
		{
			// Anim state nodes that don't support bidirectional transitions should hide this property in FAnimTransitionNodeDetails::CustomizeDetails
			if (Transition->Bidirectional)
			{
				OutTransitions.Add(Transition);
			}
		}
	}
#endif

	// Sort the transitions by priority order, lower numbers are higher priority
	if (bWantSortedList)
	{
		struct FCompareTransitionsByPriority
		{
			FORCEINLINE bool operator()(const URuleTransitionNode& A, const URuleTransitionNode& B) const
			{
				return A.PriorityOrder < B.PriorityOrder;
			}
		};

		OutTransitions.Sort(FCompareTransitionsByPriority());
	}
}

#if false
UAnimBlueprint* URuleNodeBase::GetAnimBlueprint() const
{
	UBlueprint* Blueprint = FBlueprintEditorUtils::FindBlueprintForNode(this);
	return CastChecked<UAnimBlueprint>(Blueprint);
}
#endif

