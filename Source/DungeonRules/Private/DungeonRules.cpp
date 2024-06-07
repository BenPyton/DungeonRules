/*
 * MIT License
 *
 * Copyright (c) 2024 Benoit Pelletier
 *
 * Permission is hereby granted, free of charge, to any person obtaining a copy
 * of this software and associated documentation files (the "Software"), to deal
 * in the Software without restriction, including without limitation the rights
 * to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 * copies of the Software, and to permit persons to whom the Software is
 * furnished to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included in all
 * copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 * AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 * OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
 * SOFTWARE.
 */

#include "DungeonRules.h"
#include "RoomData.h"
#include "DungeonGenerator.h"
#include "DungeonRulesLog.h"
#include "DungeonRoomChooser.h"
#include "RuleTransitionCondition.h"

bool UDungeonRuleTransition::CheckCondition(ADungeonGenerator* Generator, const URoomData* PreviousRoom) const
{
	if (!IsValid(Condition))
		return true;

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

		NextRule = Transition->NextRule.Get();
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

	for (UDungeonRule* Rule : Rules)
	{
		Rule->Clear();
	}

	Rules.Empty();
	Transitions.Empty();
}

void UDungeonRules::AddRule(UDungeonRule* Rule)
{
	check(Rule);
	Rules.AddUnique(Rule);
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
