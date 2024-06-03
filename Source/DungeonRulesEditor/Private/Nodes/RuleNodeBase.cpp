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
	FDungeonRuleNodeNameValidator(const URuleNodeBase* InNode)
		: FStringSetNameValidator(FString())
	{
		UDungeonRulesGraph* RulesGraph = CastChecked<UDungeonRulesGraph>(InNode->GetOuter());

		TArray<URuleNodeBase*> Nodes;
		RulesGraph->GetNodesOfClass<URuleNodeBase>(Nodes);
		for (URuleNodeBase* Node : Nodes)
		{
			if (Node != InNode)
			{
				Names.Add(Node->GetStateName());
			}
		}
	}
};

/////////////////////////////////////////////////////
// UDungeonRuleNodeBase

URuleNodeBase::URuleNodeBase()
	: Super()
{
}

#if false
void URuleNodeBase::PostPasteNode()
{
	Super::PostPasteNode();

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
}
#endif

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
	return MakeShareable(new FDungeonRuleNodeNameValidator(this));
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
	UEdGraphPin* OutputPin = GetOutputPin();
	if (!OutputPin)
		return;

	// Normal transitions
	for (int32 LinkIndex = 0; LinkIndex < OutputPin->LinkedTo.Num(); ++LinkIndex)
	{
		UEdGraphNode* TargetNode = OutputPin->LinkedTo[LinkIndex]->GetOwningNode();
		if (URuleTransitionNode* Transition = Cast<URuleTransitionNode>(TargetNode))
		{
			OutTransitions.Add(Transition);
		}
	}

#if false // Don't need sorted since the priority is compared at runtime
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
#endif
}

#if false
UAnimBlueprint* URuleNodeBase::GetAnimBlueprint() const
{
	UBlueprint* Blueprint = FBlueprintEditorUtils::FindBlueprintForNode(this);
	return CastChecked<UAnimBlueprint>(Blueprint);
}
#endif

