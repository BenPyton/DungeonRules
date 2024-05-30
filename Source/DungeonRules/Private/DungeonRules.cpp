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

const UDungeonRule* UDungeonRule::GetNextRule(ADungeonGenerator* Generator, URoomData* PreviousRoom) const
{
	const UDungeonRule* NextRule = this;
	int32 CurrentPriority = UINT32_MAX;
	for (const FDungeonRuleTransition& Transition : Transitions)
	{
		if (Transition.Priority >= CurrentPriority)
			continue;

		if (!Transition.CheckCondition(Generator, PreviousRoom))
			continue;

		NextRule = Transition.NextRule.Get();
		CurrentPriority = Transition.Priority;
	}

	return NextRule;
}

bool FDungeonRuleTransition::CheckCondition(ADungeonGenerator* Generator, URoomData* PreviousRoom) const
{
	if (!IsValid(Condition))
		return true;

	return Condition->Check(Generator, PreviousRoom);
}

UDungeonRules::UDungeonRules()
	: Super()
{
}

URoomData* UDungeonRules::GetFirstRoomData(ADungeonGenerator* Generator)
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

	TransitToNextRule(Generator, Room);
	return Room;
}

URoomData* UDungeonRules::GetNextRoomData(ADungeonGenerator* Generator, URoomData* PreviousRoom, const FDoorDef& DoorData, int& DoorIndex)
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

	TransitToNextRule(Generator, Room);
	return Room;
}

void UDungeonRules::TransitToNextRule(ADungeonGenerator* Generator, URoomData* Room)
{
	if (!IsValid(CurrentRule))
		return;

	const UDungeonRule* NextRule = CurrentRule->GetNextRule(Generator, Room);
	CurrentRule = NextRule;
}
