// Copyright (c) 2024 Benoit Pelletier
// SPDX-License-Identifier: BSL-1.0
// Distributed under the Boost Software License, Version 1.0. 
// (See accompanying file LICENSE or copy at https://www.boost.org/LICENSE_1_0.txt)

#include "DungeonRulesNode_Conduit.h"
#include "DungeonRulesNode_Transition.h"
#include "DungeonRulesEdTypes.h"
#include "DungeonRules.h"

#define LOCTEXT_NAMESPACE "DungeonRulesNode_Conduit"

/////////////////////////////////////////////////////
// UDungeonRulesNode_Conduit

UDungeonRulesNode_Conduit::UDungeonRulesNode_Conduit()
	: Super()
{
	bCanRenameNode = false;
}

void UDungeonRulesNode_Conduit::AllocateDefaultPins()
{
	CreatePin(EGPD_Input, DungeonRulesPinCategory::Transition, TEXT("In"));
	CreatePin(EGPD_Output, DungeonRulesPinCategory::Transition, TEXT("Out"));
}

FText UDungeonRulesNode_Conduit::GetTooltipText() const
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

const UClass* UDungeonRulesNode_Conduit::GetInstanceClass() const
{
	return URuleConduit::StaticClass();
}

UEdGraphPin* UDungeonRulesNode_Conduit::GetInputPin() const
{
	return Pins[0];
}

UEdGraphPin* UDungeonRulesNode_Conduit::GetOutputPin() const
{
	return Pins[1];
}

FString UDungeonRulesNode_Conduit::GetStateName() const
{
	return GetDesiredNewNodeName();
}

FString UDungeonRulesNode_Conduit::GetDesiredNewNodeName() const
{
	return TEXT("Conduit");
}

#undef LOCTEXT_NAMESPACE
