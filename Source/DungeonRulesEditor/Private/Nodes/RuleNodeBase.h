// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "UObject/ObjectMacros.h"
#include "EdGraph/EdGraphNode.h"
#include "RuleNodeBase.generated.h"

class INameValidatorInterface;
class UAnimBlueprint;
class UEdGraph;
class UEdGraphPin;
class UEdGraphSchema;

UCLASS(MinimalAPI, abstract)
class URuleNodeBase : public UEdGraphNode
{
	GENERATED_BODY()

public:
	URuleNodeBase();

	virtual void Serialize(FArchive& Ar) override;
	virtual void PostLoad() override;

	// UEdGraphNode interface
	virtual UObject* GetJumpTargetForDoubleClick() const override;
	virtual bool CanJumpToDefinition() const override;
	virtual void JumpToDefinition() const override;
	virtual bool CanCreateUnderSpecifiedSchema(const UEdGraphSchema* Schema) const override;
	virtual TSharedPtr<class INameValidatorInterface> MakeNameValidator() const override;
	// End of UEdGraphNode interface

	// @return the input pin for this state
	DUNGEONRULESEDITOR_API virtual UEdGraphPin* GetInputPin() const { return nullptr; }

	// @return the output pin for this state
	DUNGEONRULESEDITOR_API virtual UEdGraphPin* GetOutputPin() const { return nullptr; }

	// @return the name of this state
	DUNGEONRULESEDITOR_API virtual FString GetStateName() const { return TEXT("BaseState"); }

	// Populates the OutTransitions array with a list of transition nodes connected to this state
	DUNGEONRULESEDITOR_API virtual void GetTransitionList(TArray<class URuleTransitionNode*>& OutTransitions, bool bWantSortedList = false) const;

	virtual void PostCopyNode() {}

protected:
	// Name used as a seed when pasting nodes
	virtual FString GetDesiredNewNodeName() const { return TEXT("State"); }

#if false // TODO: documentation
public:
	// Gets the animation state node documentation link
	virtual FString GetDocumentationLink() const override;
#endif
};
