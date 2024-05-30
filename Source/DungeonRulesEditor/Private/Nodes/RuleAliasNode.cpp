// Copyright Epic Games, Inc. All Rights Reserved.

#include "RuleAliasNode.h"

#include "RuleNode.h"
#include "Containers/Array.h"
#include "EdGraph/EdGraph.h"
#include "EdGraph/EdGraphPin.h"
#include "EdGraph/EdGraphSchema.h"
#include "HAL/Platform.h"
#include "HAL/PlatformCrt.h"
#include "Internationalization/Internationalization.h"
#include "Kismet2/CompilerResultsLog.h"
#include "Kismet2/Kismet2NameValidators.h"
#include "Templates/Casts.h"
#include "Templates/ChooseClass.h"
#include "Templates/SharedPointer.h"
#include "UObject/Object.h"
#include "UObject/WeakObjectPtr.h"

class FArchive;

#define LOCTEXT_NAMESPACE "RuleAliasNode"

/////////////////////////////////////////////////////
// URuleAliasNode

URuleAliasNode::URuleAliasNode()
	: Super()
{
	bCanRenameNode = true;
	StateAliasName = TEXT("Alias");
}

void URuleAliasNode::Serialize(FArchive& Ar)
{
	Super::Serialize(Ar);
	RebuildAliasedStateNodeReferences();
}

void URuleAliasNode::AllocateDefaultPins()
{
	CreatePin(EGPD_Input, TEXT("Transition"), TEXT("In"));
	CreatePin(EGPD_Output, TEXT("Transition"), TEXT("Out"));
}

void URuleAliasNode::ValidateNodeDuringCompilation(class FCompilerResultsLog& MessageLog) const
{
	Super::ValidateNodeDuringCompilation(MessageLog);

	if ((GetInputPin()->LinkedTo.Num() > 0) && (bGlobalAlias || AliasedStateNodes.Num() != 1))
	{
		MessageLog.Error(*LOCTEXT("AliasAsEntryState", "A alias (@@) used as a transition's target must alias a single state").ToString(), this);
	}
}

void URuleAliasNode::AutowireNewNode(UEdGraphPin* FromPin)
{
	Super::AutowireNewNode(FromPin);

	if (FromPin)
	{
		if (GetSchema()->TryCreateConnection(FromPin, GetInputPin()))
		{
			FromPin->GetOwningNode()->NodeConnectionListChanged();
		}
	}
}

void URuleAliasNode::PostPasteNode()
{
	Super::PostPasteNode();

	// Find an interesting name, but try to keep the same if possible
	TSharedPtr<INameValidatorInterface> NameValidator = FNameValidatorFactory::MakeValidator(this);
	NameValidator->FindValidString(StateAliasName);
}

void URuleAliasNode::PostPlacedNewNode()
{
	// Find an interesting name, but try to keep the same if possible
	TSharedPtr<INameValidatorInterface> NameValidator = FNameValidatorFactory::MakeValidator(this);
	NameValidator->FindValidString(StateAliasName);
}

FText URuleAliasNode::GetNodeTitle(ENodeTitleType::Type TitleType) const
{
	return FText::FromString(GetStateName());
}

FText URuleAliasNode::GetTooltipText() const
{
	return LOCTEXT("ConduitNodeTooltip", "This is a conduit, which allows specification of a predicate condition for an entire group of transitions");
}

FString URuleAliasNode::GetStateName() const
{
	return StateAliasName;
}

void URuleAliasNode::OnRenameNode(const FString& NewName)
{
	StateAliasName = NewName;
}

UEdGraphPin* URuleAliasNode::GetInputPin() const
{
	return Pins[0];
}

UEdGraphPin* URuleAliasNode::GetOutputPin() const
{
	return Pins[1];
}

FString URuleAliasNode::GetDesiredNewNodeName() const
{
	return TEXT("Alias");
}

UObject* URuleAliasNode::GetJumpTargetForDoubleClick() const
{
	return GetAliasedState();
}

const TSet<TWeakObjectPtr<URuleNodeBase>>& URuleAliasNode::GetAliasedStates() const
{
	return AliasedStateNodes;
}

TSet<TWeakObjectPtr<URuleNodeBase>>& URuleAliasNode::GetAliasedStates()
{
	return AliasedStateNodes;
}

URuleNodeBase* URuleAliasNode::GetAliasedState() const
{
	// If we alias more than one state, we return null
	if (bGlobalAlias || (AliasedStateNodes.Num() != 1))
	{
		return nullptr;
	}


	if (URuleNodeBase* AliasedState = AliasedStateNodes.CreateConstIterator()->Get())
	{
		if (IsValidChecked(AliasedState))
		{
			if (const UEdGraph* Graph = GetGraph())
			{
				TArray<URuleNodeBase*> StateNodes;
				Graph->GetNodesOfClassEx<URuleNode, URuleNodeBase>(StateNodes);
				return StateNodes.Contains(AliasedState) ? AliasedState : nullptr;
			}
		}
	}
	
	return nullptr;
}

void URuleAliasNode::RebuildAliasedStateNodeReferences()
{
	TSet<TWeakObjectPtr<URuleNodeBase>> NewAliasedStateNodes;

	// We don't use UEdGraphNode::GetGraph because this may be called during deletion and we don't want to assert on a missing graph.
	if (const UEdGraph* Graph = Cast<UEdGraph>(GetOuter()))
	{
		TArray<URuleNodeBase*> StateNodes;
		Graph->GetNodesOfClassEx<URuleNode, URuleNodeBase>(StateNodes);
		TSet<URuleNodeBase*> StateNodesSet(StateNodes);

		for (auto StateNodeIt = AliasedStateNodes.CreateIterator(); StateNodeIt; ++StateNodeIt)
		{
			URuleNodeBase* AliasedState = StateNodeIt->Get();

			// Keep only nodes that are still in the graph
			if (IsValid(AliasedState) && StateNodesSet.Contains(AliasedState))
			{
				NewAliasedStateNodes.Add(*StateNodeIt);
			}
		}

		AliasedStateNodes = NewAliasedStateNodes;
	}
}

#undef LOCTEXT_NAMESPACE
