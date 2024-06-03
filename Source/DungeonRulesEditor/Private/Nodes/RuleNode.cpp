// Copyright Epic Games, Inc. All Rights Reserved.

#include "RuleNode.h"
#include "Kismet2/BlueprintEditorUtils.h"
#include "RuleTransitionNode.h"
#include "DungeonRulesGraph.h"
#include "DungeonRules.h"
#include "Kismet2/Kismet2NameValidators.h"
#include "DungeonRulesEdTypes.h"
#include "DungeonRulesEdLog.h"
#include "Internationalization/Regex.h"

#define LOCTEXT_NAMESPACE "DungeonRuleNode"

void GetNameWithoutSuffix(FString& InOutName)
{
	static const FRegexPattern SuffixPattern(TEXT("_[0-9]*$"));

	FRegexMatcher Matcher(SuffixPattern, InOutName);
	if (!Matcher.FindNext())
		return;

	int32 SuffixStartIndex = Matcher.GetCaptureGroupBeginning(0);
	InOutName.LeftInline(SuffixStartIndex);
}

FString GetNodeUniqueName(URuleNode* RuleNode, const FString& DesiredName)
{
	TSharedPtr<INameValidatorInterface> NameValidator = FNameValidatorFactory::MakeValidator(RuleNode);
	FString UniqueName(DesiredName);
	//GetNameWithoutSuffix(UniqueName);
	EValidatorResult Result = NameValidator->FindValidString(UniqueName);
	return UniqueName;
}

URuleNode::URuleNode()
	: Super()
{
	bCanRenameNode = true;
}

void URuleNode::AllocateDefaultPins()
{
	UEdGraphPin* Inputs = CreatePin(EGPD_Input, DungeonRulesPinCategory::Transition, FName("In"));
	UEdGraphPin* Outputs = CreatePin(EGPD_Output, DungeonRulesPinCategory::Transition, FName("Out"));
}

void URuleNode::AutowireNewNode(UEdGraphPin* FromPin)
{
	Super::AutowireNewNode(FromPin);

	//@TODO: If the FromPin is a state, create a transition between us
	if (FromPin)
	{
		if (GetSchema()->TryCreateConnection(FromPin, GetInputPin()))
		{
			FromPin->GetOwningNode()->NodeConnectionListChanged();
		}
	}
}

FText URuleNode::GetNodeTitle(ENodeTitleType::Type TitleType) const
{
	return FText::FromString(GetStateName());
}

FText URuleNode::GetTooltipText() const
{
	return LOCTEXT("DungeonRuleNode_Tooltip", "This is a state");
}

FString URuleNode::GetStateName() const
{
	return RuleInstance ? RuleInstance->RuleName : TEXT("NULL");
}

void URuleNode::PostCopyNode()
{
	ResetInstanceOwner();
}

#if WITH_EDITOR

void URuleNode::PostEditImport()
{
	ResetInstanceOwner();
#if false // TODO: move to URuleNodeBase
	if (NodeInstance)
	{
		InitializeInstance();
	}
#endif
}

void URuleNode::PostEditUndo()
{
	UEdGraphNode::PostEditUndo();
	ResetInstanceOwner();
}

#endif

void URuleNode::ResetInstanceOwner()
{
	if (RuleInstance)
	{
		UEdGraph* MyGraph = GetGraph();
		UObject* GraphOwner = MyGraph ? MyGraph->GetOuter() : nullptr;

		RuleInstance->Rename(nullptr, GraphOwner, REN_DontCreateRedirectors | REN_DoNotDirty);
		RuleInstance->ClearFlags(RF_Transient);
	}
}

UEdGraphPin* URuleNode::GetInputPin() const
{
	return Pins[0];
}


UEdGraphPin* URuleNode::GetOutputPin() const
{
	return Pins[1];
}

void URuleNode::PostPasteNode()
{
	if (RuleInstance)
	{
		// Deep copy the pasted rule instance
		CreateInstance(RuleInstance);
	}
	Super::PostPasteNode();
}

void URuleNode::PostPlacedNewNode()
{
	if (RuleInstance)
		return;

	CreateInstance();
}

void URuleNode::PrepareForCopying()
{
	if (RuleInstance)
	{
		// Temporarily take ownership of the node instance, so that it is not deleted when cutting
		RuleInstance->Rename(nullptr, this, REN_DontCreateRedirectors | REN_DoNotDirty);
	}
}

void URuleNode::OnRenameNode(const FString& NewName)
{
	if (!RuleInstance)
		return;

	RuleInstance->Modify();
	RuleInstance->RuleName = NewName;
}

void URuleNode::CreateInstance(const UDungeonRule* Template)
{
	UEdGraph* Graph = GetGraph();
	UObject* GraphOwner = Graph ? Graph->GetOuter() : nullptr;
	if (!GraphOwner)
		return;

	if (!Template)
	{
		RuleInstance = NewObject<UDungeonRule>(GraphOwner);
		RuleInstance->SetFlags(RF_Transactional);
		RuleInstance->RuleName = GetNodeUniqueName(this, TEXT("New Rule"));
	}
	else
	{
		RuleInstance = DuplicateObject(Template, GraphOwner);
		RuleInstance->RuleName = GetNodeUniqueName(this, RuleInstance->RuleName);
	}
}

#undef LOCTEXT_NAMESPACE
