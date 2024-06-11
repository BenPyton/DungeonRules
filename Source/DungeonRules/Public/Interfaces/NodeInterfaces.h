// Copyright (c) 2024 Benoit Pelletier
// SPDX-License-Identifier: BSL-1.0
// Distributed under the Boost Software License, Version 1.0. 
// (See accompanying file LICENSE or copy at https://www.boost.org/LICENSE_1_0.txt)

#pragma once

#include "CoreMinimal.h"
#include "UObject/Interface.h"
#include "NodeInterfaces.generated.h"

UINTERFACE(MinimalAPI, meta = (CannotImplementInterfaceInBlueprint))
class UNodeName : public UInterface
{
	GENERATED_BODY()
};

class DUNGEONRULES_API INodeName
{
	GENERATED_BODY()
public:
	virtual FString GetNodeName() const = 0;
	virtual void OnNodeRename(FString NewName) = 0;
};

/////////////////////////////////////////

UINTERFACE(MinimalAPI, meta = (CannotImplementInterfaceInBlueprint))
class UNodeTooltip : public UInterface
{
	GENERATED_BODY()
};

class DUNGEONRULES_API INodeTooltip
{
	GENERATED_BODY()
public:
	virtual FText GetNodeTooltip() const = 0;
};
