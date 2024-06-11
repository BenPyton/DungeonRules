// Copyright (c) 2024 Benoit Pelletier
// SPDX-License-Identifier: BSL-1.0
// Distributed under the Boost Software License, Version 1.0. 
// (See accompanying file LICENSE or copy at https://www.boost.org/LICENSE_1_0.txt)

#include "DungeonRoomChooser.h"
#include "DungeonRulesLog.h"

URoomData* UDungeonRoomChooser::ChooseFirstRoomData_Implementation(ADungeonGenerator* Generator) const
{
	RulesLog_Error("Choose First Room is not implemented in %s.", *GetNameSafe(this));
	return nullptr;
}

URoomData* UDungeonRoomChooser::ChooseNextRoomData_Implementation(ADungeonGenerator* Generator, const URoomData* PreviousRoom, const FDoorDef& DoorData, int& DoorIndex) const
{
	RulesLog_Error("Choose Next Room is not implemented in %s.", *GetNameSafe(this));
	return nullptr;
}

FText UDungeonRoomChooser::GetDescription_Implementation() const
{
#if WITH_EDITOR
	return FText::Format(NSLOCTEXT("DungeonRules", "NoRoomChooserDescription", "No description provided.\nOverride 'Get Description' in '{0}'."), GetClass()->GetDisplayNameText());
#else
	return FText();
#endif
}
