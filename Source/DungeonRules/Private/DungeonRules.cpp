// Copyright (c) 2024 Benoit Pelletier
// SPDX-License-Identifier: BSL-1.0
// Distributed under the Boost Software License, Version 1.0. 
// (See accompanying file LICENSE or copy at https://www.boost.org/LICENSE_1_0.txt)

#include "DungeonRules.h"
#include "RoomData.h"
#include "DungeonGenerator.h"
#include "DungeonRulesLog.h"
#include "DungeonRoomChooser.h"
#include "RuleTransitionCondition.h"

bool UDungeonRuleTransition::CheckCondition(ADungeonGenerator* Generator, const URoomData* PreviousRoom) const
{
	UE_LOG(LogTemp, Log, TEXT("[%s] Transition Check Condition"), *GetNameSafe(PreviousRoom));
	if (const IDungeonConditionProvider* ConditionProvider = Cast<IDungeonConditionProvider>(NextRule.GetObject()))
	{
		UE_LOG(LogTemp, Log, TEXT("NextRule has condition"));
		// If the next state has a condition and is not fulfilled, then we can't go into it.
		if (!ConditionProvider->CheckCondition(Generator, PreviousRoom))
		{
			UE_LOG(LogTemp, Log, TEXT("Condition not valid"));
			return false;
		}
	}

	// If this transition has no condition, then it goes always to the next state.
	if (!IsValid(Condition))
	{
		UE_LOG(LogTemp, Log, TEXT("No condition"));
		return true;
	}

	UE_LOG(LogTemp, Log, TEXT("Check Condition from transition"));
	return Condition->Check(Generator, PreviousRoom);
}

FText UDungeonRuleTransition::GetNodeTooltip() const
{
	if (!IsValid(Condition))
		return NSLOCTEXT("UDungeonRuleTransition", "TransitionNoCondition", "Always true.");

	return Condition->GetDescription();
}

TOptional<const UDungeonRule*> UDungeonRuleTransition::GetNextRuleFromTransitionList(ADungeonGenerator* Generator, const URoomData* PreviousRoom, const TArray<TWeakObjectPtr<const UDungeonRuleTransition>>& Transitions, const UObject* Context /*= nullptr*/)
{
	TOptional<const UDungeonRule*> NextRule;
	int32 CurrentPriority = INT32_MAX;
	for (const auto& Transition : Transitions)
	{
		if (!Transition.IsValid())
		{
			RulesLog_Warning("Invalid transition found in %s.", *GetNameSafe(Context));
			continue;
		}

		if (Transition->PriorityOrder >= CurrentPriority)
			continue;

		if (!Transition->CheckCondition(Generator, PreviousRoom))
			continue;

		const IDungeonRuleProvider* RuleProvider = Transition->NextRule.GetInterface();
		NextRule = RuleProvider ? RuleProvider->GetRule(Generator, PreviousRoom) : nullptr;
		CurrentPriority = Transition->PriorityOrder;
	}

	return NextRule;
}

//////////////////////////////////////////////////////////////////////

TOptional<const UDungeonRule*> UDungeonRule::GetNextRule(ADungeonGenerator* Generator, const URoomData* PreviousRoom) const
{
	return UDungeonRuleTransition::GetNextRuleFromTransitionList(Generator, PreviousRoom, Transitions, this);
}

FText UDungeonRule::GetNodeTooltip() const
{
	if (!IsValid(RoomChooser))
		return NSLOCTEXT("UDungeonRule", "NoRoomChooser", "Return None");

	return RoomChooser->GetDescription();
}

#if WITH_EDITOR
void UDungeonRule::Clear()
{
	Transitions.Empty();
}

void UDungeonRule::AddTransition(const UDungeonRuleTransition* Transition)
{
	check(Transition);
	Transitions.AddUnique(Transition);
}
#endif

///////////////////////////////////////////////////////////////////

const UDungeonRule* URuleConduit::GetRule(ADungeonGenerator* Generator, const URoomData* PreviousRoom) const
{
	auto NextRule = UDungeonRuleTransition::GetNextRuleFromTransitionList(Generator, PreviousRoom, Transitions, this);
	return NextRule.IsSet() ? NextRule.GetValue() : nullptr;
}

bool URuleConduit::CheckCondition(ADungeonGenerator* Generator, const URoomData* PreviousRoom) const
{
	UE_LOG(LogTemp, Log, TEXT("[%s] Conduit Check Condition"), *GetNameSafe(PreviousRoom));
	// Returns true if at least one output is valid.
	for (const auto& Transition : Transitions)
	{
		if (Transition->CheckCondition(Generator, PreviousRoom))
			return true;
	}
	return false;
}

#if WITH_EDITOR
void URuleConduit::Clear()
{
	Transitions.Empty();
}

void URuleConduit::AddTransition(const UDungeonRuleTransition* Transition)
{
	check(Transition);
	Transitions.AddUnique(Transition);
}
#endif

///////////////////////////////////////////////////////////////////

UDungeonRules::UDungeonRules()
	: Super()
{
}

URoomData* UDungeonRules::GetFirstRoomData(ADungeonGenerator* Generator, const UDungeonRule* CurrentRule) const
{
	if (!IsValid(CurrentRule))
	{
		RulesLog_Error("No current rule!");
		return nullptr;
	}

	UDungeonRoomChooser* RoomChooser = CurrentRule->RoomChooser;
	if (!IsValid(RoomChooser))
	{
		RulesLog_Error("No room chooser in current rule!");
		return nullptr;
	}

	URoomData* Room = RoomChooser->ChooseFirstRoomData(Generator);
	if (!IsValid(Room))
	{
		RulesLog_Error("Room chooser in current rule returned invalid room data!");
		return nullptr;
	}

	return Room;
}

URoomData* UDungeonRules::GetNextRoomData(ADungeonGenerator* Generator, const UDungeonRule* CurrentRule, const URoomData* PreviousRoom, const FDoorDef& DoorData, int& DoorIndex) const
{
	if (!IsValid(CurrentRule))
	{
		RulesLog_Error("No current rule!");
		return nullptr;
	}

	UDungeonRoomChooser* RoomChooser = CurrentRule->RoomChooser;
	if (!IsValid(RoomChooser))
	{
		RulesLog_Error("No room chooser in current rule!");
		return nullptr;
	}

	URoomData* Room = RoomChooser->ChooseNextRoomData(Generator, PreviousRoom, DoorData, DoorIndex);
	if (!IsValid(Room))
	{
		RulesLog_Error("Room chooser in current rule returned invalid room data!");
		return nullptr;
	}

	return Room;
}

const UDungeonRule* UDungeonRules::GetNextRule(ADungeonGenerator* Generator, const UDungeonRule* CurrentRule, const URoomData* Room) const
{
	if (!IsValid(CurrentRule))
		return nullptr;

	TOptional<const UDungeonRule*> NextRule = CurrentRule->GetNextRule(Generator, Room);
	if (NextRule.IsSet())
		return NextRule.GetValue();

	NextRule = UDungeonRuleTransition::GetNextRuleFromTransitionList(Generator, Room, GlobalTransitions, this);
	return NextRule.IsSet() ? NextRule.GetValue() : CurrentRule;
}

#if WITH_EDITOR
void UDungeonRules::Clear()
{
	FirstRule.Reset();
	GlobalTransitions.Empty();

	for (URuleConduit* Conduit : Conduits)
	{
		Conduit->Clear();
	}

	for (UDungeonRule* Rule : Rules)
	{
		Rule->Clear();
	}

	Transitions.Empty();
	Conduits.Empty();
	Rules.Empty();
}

void UDungeonRules::AddRule(UDungeonRule* Rule)
{
	check(Rule);
	Rules.AddUnique(Rule);
}

void UDungeonRules::AddConduit(URuleConduit* Conduit)
{
	check(Conduit);
	Conduits.AddUnique(Conduit);
}

void UDungeonRules::SetFirstRule(UDungeonRule* Rule)
{
	check(Rules.Contains(Rule));
	FirstRule = Rule;
}

void UDungeonRules::AddTransition(UDungeonRuleTransition* Transition)
{
	check(Transition);
	Transitions.AddUnique(Transition);
}

void UDungeonRules::AddGlobalTransition(UDungeonRuleTransition* GlobalTransition)
{
	check(Transitions.Contains(GlobalTransition));
	GlobalTransitions.Add(GlobalTransition);
}
#endif
