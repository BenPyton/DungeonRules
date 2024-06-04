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
#include "DungeonRules.h"
#include "Internationalization/Regex.h"

/////////////////////////////////////////////////////
// FDungeonRuleNodeNameValidator

void GetNameWithoutSuffix(FString& InOutName)
{
	static const FRegexPattern SuffixPattern(TEXT("_[0-9]*$"));

	FRegexMatcher Matcher(SuffixPattern, InOutName);
	if (!Matcher.FindNext())
		return;

	int32 SuffixStartIndex = Matcher.GetCaptureGroupBeginning(0);
	InOutName.LeftInline(SuffixStartIndex);
}

FString GetNodeUniqueName(UEdGraphNode* Node, const FString& DesiredName)
{
	TSharedPtr<INameValidatorInterface> NameValidator = FNameValidatorFactory::MakeValidator(Node);
	FString UniqueName(DesiredName);
	//GetNameWithoutSuffix(UniqueName);
	EValidatorResult Result = NameValidator->FindValidString(UniqueName);
	return UniqueName;
}

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

UObject* URuleNodeBase::GetJumpTargetForDoubleClick() const
{
	return nullptr;
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

bool URuleNodeBase::CanCreateUnderSpecifiedSchema(const UEdGraphSchema* Schema) const
{
	return Schema->IsA(UDungeonRulesSchema::StaticClass());
}

TSharedPtr<class INameValidatorInterface> URuleNodeBase::MakeNameValidator() const
{
	return MakeShareable(new FDungeonRuleNodeNameValidator(this));
}

FText URuleNodeBase::GetTooltipText() const
{
	if (INodeTooltip* TooltipInterface = Cast<INodeTooltip>(NodeInstance))
		return TooltipInterface->GetNodeTooltip();

	return Super::GetTooltipText();
}

void URuleNodeBase::PrepareForCopying()
{
	if (NodeInstance)
	{
		// Temporarily take ownership of the node instance, so that it is not deleted when cutting
		NodeInstance->Rename(nullptr, this, REN_DontCreateRedirectors | REN_DoNotDirty);
	}
}

#if false // TODO: documentation
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

void URuleNodeBase::PostPasteNode()
{
	if (NodeInstance)
	{
		// Deep copy the pasted rule instance
		CreateInstance(GetNodeInstance());
	}
	Super::PostPasteNode();
}

void URuleNodeBase::PostPlacedNewNode()
{
	if (NodeInstance)
		return;

	CreateInstance();
}

void URuleNodeBase::OnRenameNode(const FString& NewName)
{
	INodeName* NameInterface = Cast<INodeName>(NodeInstance);
	if (!NameInterface)
		return;

	NodeInstance->Modify();
	NameInterface->OnNodeRename(NewName);
}

void URuleNodeBase::PostCopyNode()
{
	ResetInstanceOwner();
}

#if WITH_EDITOR

void URuleNodeBase::PostEditImport()
{
	ResetInstanceOwner();
	if (NodeInstance)
		InitializeInstance();
}

void URuleNodeBase::PostEditUndo()
{
	UEdGraphNode::PostEditUndo();
	ResetInstanceOwner();
}

#endif

void URuleNodeBase::CreateInstance(const UObject* Template)
{
	UEdGraph* Graph = GetGraph();
	UObject* GraphOwner = Graph ? Graph->GetOuter() : nullptr;
	if (!GraphOwner)
		return;

	FString Name;
	if (!Template)
	{
		const UClass* InstanceClass = GetInstanceClass();
		if (!InstanceClass)
			return;

		NodeInstance = NewObject<UObject>(GraphOwner, InstanceClass);
		NodeInstance->SetFlags(RF_Transactional);
		Name = GetDesiredNewNodeName();
	}
	else
	{
		NodeInstance = DuplicateObject(Template, GraphOwner);
		if (INodeName* NameInterface = Cast<INodeName>(NodeInstance))
			Name = NameInterface->GetNodeName();
		else
			Name = NodeInstance->GetName();
	}

	Name = GetNodeUniqueName(this, Name);
	if (INodeName* NameInterface = Cast<INodeName>(NodeInstance))
		NameInterface->OnNodeRename(Name);

	InitializeInstance();
}

void URuleNodeBase::ResetInstanceOwner()
{
	if (NodeInstance)
	{
		UEdGraph* MyGraph = GetGraph();
		UObject* GraphOwner = MyGraph ? MyGraph->GetOuter() : nullptr;

		NodeInstance->Rename(nullptr, GraphOwner, REN_DontCreateRedirectors | REN_DoNotDirty);
		NodeInstance->ClearFlags(RF_Transient);
	}
}
