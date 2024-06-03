// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "EdGraph/EdGraph.h"
#include "UObject/ObjectMacros.h"
#include "UObject/ObjectPtr.h"
#include "UObject/UObjectGlobals.h"

#include "DungeonRulesGraph.generated.h"

//class UObject;

UCLASS(MinimalAPI)
class UDungeonRulesGraph : public UEdGraph
{
	GENERATED_BODY()
public:

	UDungeonRulesGraph();

	// Entry node within the state machine
	UPROPERTY()
	TObjectPtr<class URuleEntryNode> EntryNode;

	UPROPERTY()
	TObjectPtr<class URuleExitNode> ExitNode;

	virtual void OnSave();
	//virtual void RefreshNodes(); // used in DungeonRulesToolkit.cpp --- Is Needed?

	// Parent instance node
	//UPROPERTY()
	//TObjectPtr<class UAnimGraphNode_StateMachineBase> OwnerAnimGraphNode;

	UPROPERTY()
	int32 GraphVersion;

	virtual void OnCreated();
#if false
	virtual void OnLoaded();
#endif
	virtual void Initialize();

	virtual void UpdateAsset(int32 UpdateFlags = 0);
	virtual void UpdateVersion();
	virtual void MarkVersion();

	virtual void OnNodesPasted(const FString& ImportStr);

#if false
	bool UpdateUnknownNodeClasses();
	void UpdateDeprecatedClasses();
	void RemoveOrphanedNodes();
	void UpdateClassData();
#endif

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

#if false
	virtual void CollectAllNodeInstances(TSet<UObject*>& NodeInstances);
	virtual bool CanRemoveNestedObject(UObject* TestObject) const;
#endif
	virtual void OnNodeInstanceRemoved(UObject* NodeInstance);

	UEdGraphPin* FindGraphNodePin(UEdGraphNode* Node, EEdGraphPinDirection Dir);
};

