// Copyright (c) 2024 Benoit Pelletier
// SPDX-License-Identifier: BSL-1.0
// Distributed under the Boost Software License, Version 1.0. 
// (See accompanying file LICENSE or copy at https://www.boost.org/LICENSE_1_0.txt)

#include "RuleConduitNode.h"
#include "Kismet2/BlueprintEditorUtils.h"
#include "RuleTransitionNode.h"
#include "Kismet2/Kismet2NameValidators.h"
#include "Kismet2/CompilerResultsLog.h"
#include "DungeonRulesEdTypes.h"
#include "DungeonRules.h"

#define LOCTEXT_NAMESPACE "RuleConduitNode"

/////////////////////////////////////////////////////
// URuleConduitNode

URuleConduitNode::URuleConduitNode()
	: Super()
{
	bCanRenameNode = false;
}

void URuleConduitNode::AllocateDefaultPins()
{
	CreatePin(EGPD_Input, DungeonRulesPinCategory::Transition, TEXT("In"));
	CreatePin(EGPD_Output, DungeonRulesPinCategory::Transition, TEXT("Out"));
}

FText URuleConduitNode::GetTooltipText() const
{
	const UEdGraphPin* Input = GetInputPin();
	if (Input->LinkedTo.Num() <= 0)
		return LOCTEXT("ConduitNodeTooltip_NoInput", "No input. This path will not be reached.");

	const UEdGraphPin* Output = GetOutputPin();
	if (Output->LinkedTo.Num() <= 0)
		return LOCTEXT("ConduitNodeTooltip_NoOutput", "No output. This path is a dead end.");

	FText TextOne = LOCTEXT("ConduitNodeTooltip_OneConnection", "One");
	FText TextMany = LOCTEXT("ConduitNodeTooltip_ManyConnections", "Many");

	const FText& InputConnections = (Input->LinkedTo.Num() == 1) ? TextOne : TextMany;
	const FText& OutputConnections = (Output->LinkedTo.Num() == 1) ? TextOne : TextMany;

	return FText::Format(LOCTEXT("ConduitNodeTooltip", "{0} to {1} conduit."), InputConnections, OutputConnections);
}

const UClass* URuleConduitNode::GetInstanceClass() const
{
	return URuleConduit::StaticClass();
}

UEdGraphPin* URuleConduitNode::GetInputPin() const
{
	return Pins[0];
}

UEdGraphPin* URuleConduitNode::GetOutputPin() const
{
	return Pins[1];
}

FString URuleConduitNode::GetStateName() const
{
	return GetDesiredNewNodeName();
}

FString URuleConduitNode::GetDesiredNewNodeName() const
{
	return TEXT("Conduit");
}

#undef LOCTEXT_NAMESPACE
