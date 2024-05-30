// Copyright Epic Games, Inc. All Rights Reserved.

#include "DungeonRulesVisualFactories.h"

//#include "AnimGraphConnectionDrawingPolicy.h"
#include "Nodes/RuleAliasNode.h"
#include "Nodes/RuleConduitNode.h"
#include "Nodes/RuleEntryNode.h"
#include "Nodes/RuleNode.h"
#include "Nodes/RuleTransitionNode.h"
//#include "DungeonRulesGraphSchema.h"
#include "DungeonRulesSchema.h"
#include "NodeSlates/SGraphNodeDungeonRule.h"
#include "NodeSlates/SGraphNodeDungeonRuleAlias.h"
#include "NodeSlates/SGraphNodeDungeonRuleEntry.h"
#include "NodeSlates/SGraphNodeDungeonRuleTransition.h"
#include "EdGraph/EdGraphNode.h"
#include "EdGraph/EdGraphPin.h"
#include "EdGraph/EdGraphSchema.h"
#include "EdGraphSchema_K2.h"
//#include "K2Node_AnimNodeReference.h"
#include "KismetPins/SGraphPinExec.h"
#include "DungeonRulesConnectionDrawingPolicy.h"
#include "Templates/Casts.h"
#include "UObject/Class.h"
#include "UObject/NameTypes.h"
#include "UObject/WeakObjectPtr.h"
#include "UObject/WeakObjectPtrTemplates.h"
#include "Widgets/DeclarativeSyntaxSupport.h"
#include "DungeonRulesEdTypes.h"

TSharedPtr<class SGraphNode> FDungeonRulesNodeFactory::CreateNode(class UEdGraphNode* InNode) const 
{
	if (URuleTransitionNode* TransitionNode = Cast<URuleTransitionNode>(InNode))
	{
		return SNew(SGraphNodeDungeonRuleTransition, TransitionNode);
	}
	else if (URuleNode* StateNode = Cast<URuleNode>(InNode))
	{
		return SNew(SGraphNodeDungeonRule, StateNode);
	}
	else if (URuleAliasNode* StateAliasNode = Cast<URuleAliasNode>(InNode))
	{
		return SNew(SGraphNodeDungeonRuleAlias, StateAliasNode);
	}
	else if (URuleConduitNode* ConduitNode = Cast<URuleConduitNode>(InNode))
	{
		return SNew(SGraphNodeDungeonRuleConduit, ConduitNode);
	}
	else if (URuleEntryNode* EntryNode = Cast<URuleEntryNode>(InNode))
	{
		return SNew(SGraphNodeDungeonRuleEntry, EntryNode);
	}
#if false
	else if (UK2Node_AnimNodeReference* AnimNodeReference = Cast<UK2Node_AnimNodeReference>(InNode))
	{
		return SNew(SAnimNodeReference, AnimNodeReference);
	}
#endif
	
	return nullptr;
}

TSharedPtr<class SGraphPin> FDungeonRulesPinFactory::CreatePin(class UEdGraphPin* InPin) const
{
#if false
	if (InPin->GetSchema()->IsA<UDungeonRulesGraphSchema>() && InPin->PinType.PinCategory == UEdGraphSchema_K2::PC_Struct)
	{
		if ((InPin->PinType.PinSubCategoryObject == FPoseLink::StaticStruct()) || (InPin->PinType.PinSubCategoryObject == FComponentSpacePoseLink::StaticStruct()))
		{
			return SNew(SGraphPinPose, InPin);
		}
	}
#endif

	if (InPin->GetSchema()->IsA<UDungeonRulesSchema>() && InPin->PinType.PinCategory == DungeonRulesPinCategory::Exec)
	{
		return SNew(SGraphPinExec, InPin);
	}

	return nullptr;
}

class FConnectionDrawingPolicy* FDungeonRulesPinConnectionFactory::CreateConnectionPolicy(const class UEdGraphSchema* Schema, int32 InBackLayerID, int32 InFrontLayerID, float ZoomFactor, const class FSlateRect& InClippingRect, class FSlateWindowElementList& InDrawElements, class UEdGraph* InGraphObj) const
{
#if false
	if (Schema->IsA(UDungeonRulesGraphSchema::StaticClass()))
	{
		return new FAnimGraphConnectionDrawingPolicy(InBackLayerID, InFrontLayerID, ZoomFactor, InClippingRect, InDrawElements, InGraphObj);
	}
	else
#endif
	if (Schema->IsA(UDungeonRulesSchema::StaticClass()))
	{
		return new FDungeonRulesConnectionDrawingPolicy(InBackLayerID, InFrontLayerID, ZoomFactor, InClippingRect, InDrawElements, InGraphObj);
	}

	return nullptr;
}
