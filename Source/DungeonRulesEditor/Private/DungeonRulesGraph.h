// Copyright (c) 2024 Benoit Pelletier
// SPDX-License-Identifier: BSL-1.0
// Distributed under the Boost Software License, Version 1.0. 
// (See accompanying file LICENSE or copy at https://www.boost.org/LICENSE_1_0.txt)

#pragma once

#include "CoreMinimal.h"
#include "EdGraph/EdGraph.h"
#include "DungeonRulesGraph.generated.h"

UCLASS(MinimalAPI)
class UDungeonRulesGraph : public UEdGraph
{
	GENERATED_BODY()
public:

	UDungeonRulesGraph();

	// Entry node within the state machine
	UPROPERTY()
	TObjectPtr<class UDungeonRulesNode_Begin> BeginNode;

	UPROPERTY()
	TObjectPtr<class UDungeonRulesNode_Stop> StopNode;

	virtual void OnSave();

	UPROPERTY()
	int32 GraphVersion;

	virtual void OnCreated();
	virtual void Initialize();

	virtual void UpdateAsset(int32 UpdateFlags = 0);
	virtual void UpdateVersion();
	virtual void MarkVersion();

	virtual void OnNodesPasted(const FString& ImportStr);

	bool IsLocked() const;
	void LockUpdates();
	void UnlockUpdates();

	//~ Begin UObject Interface.
	virtual void Serialize(FArchive& Ar) override;
	//~ End UObject Interface.

protected:

	/** if set, graph modifications won't cause updates in internal tree structure
	 *  flag allows freezing update during heavy changes like pasting new nodes
	 */
	uint32 bLockUpdates : 1;

	virtual void OnNodeInstanceRemoved(UObject* NodeInstance);

	UEdGraphPin* FindGraphNodePin(UEdGraphNode* Node, EEdGraphPinDirection Dir);
};

