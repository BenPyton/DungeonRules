// Copyright Epic Games, Inc. All Rights Reserved.

#include "SGraphNodeDungeonRuleAlias.h"

#include "Nodes/RuleAliasNode.h"
#include "Nodes/RuleNodeBase.h"
#include "Animation/AnimBlueprint.h"
#include "Animation/AnimBlueprintGeneratedClass.h"
#include "Animation/AnimInstance.h"
#include "DungeonRulesGraph.h"
#include "SGraphNodeDungeonRule.h"
#include "SGraphNodeDungeonRuleTransition.h"
#include "Containers/ContainerAllocationPolicies.h"
#include "Containers/Map.h"
#include "Containers/Set.h"
#include "EdGraph/EdGraphNode.h"
#include "Engine/Blueprint.h"
#include "HAL/PlatformCrt.h"
#include "Internationalization/Internationalization.h"
#include "Kismet2/BlueprintEditorUtils.h"
#include "Math/UnrealMathSSE.h"
#include "Styling/AppStyle.h"
#include "Templates/Casts.h"
#include "UObject/WeakObjectPtr.h"
#include "UObject/WeakObjectPtrTemplates.h"
#include "Nodes/RuleAliasNode.h"
#include "Nodes/RuleNodeBase.h"

class SToolTip;
struct FGraphInformationPopupInfo;
struct FNodeInfoContext;
struct FSlateBrush;

#define LOCTEXT_NAMESPACE "SGraphNodeDungeonRuleAlias"

/////////////////////////////////////////////////////
// SGraphNodeDungeonRuleAlias

void SGraphNodeDungeonRuleAlias::Construct(const FArguments& InArgs, URuleAliasNode* InNode)
{
	SGraphNodeDungeonRule::Construct(SGraphNodeDungeonRule::FArguments(), InNode);
}

bool SGraphNodeDungeonRuleAlias::IsNameReadOnly() const
{
	if (URuleAliasNode* AliasNode = Cast<URuleAliasNode>(GraphNode))
	{
		if (AliasNode->bGlobalAlias
			|| AliasNode->GetAliasedState()
			|| AliasNode->GetAliasedStates().Num() <= 0)
			return true;
	}

	return SGraphNode::IsNameReadOnly();
}

FSlateColor SGraphNodeDungeonRuleAlias::GetBorderBackgroundColor_Internal(FLinearColor InactiveStateColor, FLinearColor ActiveStateColorDim, FLinearColor ActiveStateColorBright) const
{
	// Override inactive state color for aliases.
	return FLinearColor(0.45f, 0.33f, 0.37f);
}

FText SGraphNodeDungeonRuleAlias::GetPreviewCornerText() const
{
	return FText();
}

const FSlateBrush* SGraphNodeDungeonRuleAlias::GetNameIcon() const
{
	return FAppStyle::GetBrush(TEXT("Graph.AliasNode.Icon"));
}

FText SGraphNodeDungeonRuleAlias::GetNodeName() const
{
	if (URuleAliasNode* AliasNode = Cast<URuleAliasNode>(GraphNode))
	{
		if (AliasNode->bGlobalAlias)
			return FText::FromString(TEXT("Any"));

		if (URuleNodeBase* AliasedNode = AliasNode->GetAliasedState())
			return FText::FromString(FString::Printf(TEXT("@%s"), *AliasedNode->GetStateName()));

		if (AliasNode->GetAliasedStates().Num() <= 0)
			return FText::FromString(TEXT("None"));
	}

	return SGraphNodeDungeonRule::GetNodeName();
}

#undef LOCTEXT_NAMESPACE
