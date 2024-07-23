// Copyright (c) 2024 Chat Cripant


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

void UDungeonEventReceiver::OnRoomAdded_Implementation(ADungeonGenerator* Generator, const URoomData* NewRoom)
{
}

void UDungeonEventReceiver::OnFailedToAddRoom_Implementation(ADungeonGenerator* Generator, const URoomData* FromRoom, const FDoorDef& FromDoor)
{
}
