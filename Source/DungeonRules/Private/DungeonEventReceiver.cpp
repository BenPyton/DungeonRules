/// Copyright (c) 2024 Benoit Pelletier
// SPDX-License-Identifier: BSL-1.0
// Distributed under the Boost Software License, Version 1.0. 
// (See accompanying file LICENSE or copy at https://www.boost.org/LICENSE_1_0.txt)

#include "DungeonEventReceiver.h"

void UDungeonEventReceiver::OnPreGeneration_Implementation(ADungeonGenerator* Generator)
{
}

void UDungeonEventReceiver::OnPostGeneration_Implementation(ADungeonGenerator* Generator)
{
}

void UDungeonEventReceiver::OnGenerationInit_Implementation(ADungeonGenerator* Generator)
{
}

void UDungeonEventReceiver::OnGenerationFailed_Implementation(ADungeonGenerator* Generator)
{
}

void UDungeonEventReceiver::OnRoomAdded_Implementation(ADungeonGenerator* Generator, const TScriptInterface<IReadOnlyRoom>& NewRoom)
{
}

void UDungeonEventReceiver::OnFailedToAddRoom_Implementation(ADungeonGenerator* Generator, const URoomData* FromRoom, const FDoorDef& FromDoor)
{
}
