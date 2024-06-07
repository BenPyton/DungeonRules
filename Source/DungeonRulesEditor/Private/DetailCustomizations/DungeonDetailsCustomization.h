// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "IDetailCustomization.h"
#include "Toolkits/DungeonRulesToolkit.h"

class SEditableTextBox;
class SMultiLineEditableTextBox;

// This is almost the same as the FBlueprintGraphNodeDetails (in BlueprintDetailsCustomization.h)
// The only change is that it doesn't use an FBlueprintEditor, but an FDungeonRulesToolkit.
// Too bad there is not a common interface to use for the node names...
class FEditorGraphNodeNameDetails : public IDetailCustomization
{
public:
	/** Constructor */
	FEditorGraphNodeNameDetails(FDungeonRulesToolkit* InToolkitPtr)
		: GraphNodePtr(nullptr)
		, ToolkitPtr(InToolkitPtr)
	{
	}

	/** Makes a new instance of this detail layout class for a specific detail view requesting it */
	static TSharedRef<class IDetailCustomization> MakeInstance(FDungeonRulesToolkit* InToolkitPtr)
	{
		return MakeShareable(new FEditorGraphNodeNameDetails(InToolkitPtr));
	}

	/** IDetailCustomization interface */
	virtual void CustomizeDetails( IDetailLayoutBuilder& DetailLayout ) override;

private:

	/** Set error to name textbox */
	void SetNameError( const FText& Error );

	// Callbacks for uproperty details customization
	FText OnGetName() const;
	bool IsNameReadOnly() const;
	void OnNameChanged(const FText& InNewText);
	void OnNameCommitted(const FText& InNewName, ETextCommit::Type InTextCommit);

	/** The widget used when editing a singleline name */ 
	TSharedPtr<SEditableTextBox> NameEditableTextBox;
	/** The widget used when editing a multiline name */ 
	TSharedPtr<SMultiLineEditableTextBox> MultiLineNameEditableTextBox;
	/** The target GraphNode */
	TWeakObjectPtr<UEdGraphNode> GraphNodePtr;
	/** Weak reference to the Blueprint editor */
	FDungeonRulesToolkit* ToolkitPtr {nullptr};
};

/** Details customization for URuleNodeBase instances */
class FDungeonRuleNodeBaseDetails : public IDetailCustomization
{
public:
	FDungeonRuleNodeBaseDetails()
		: RuleNodePtr(nullptr)
	{
	}

	static TSharedRef<class IDetailCustomization> MakeInstance()
	{
		return MakeShareable(new FDungeonRuleNodeBaseDetails());
	}

	//~ Begin IDetailCustomization Interface
	virtual void CustomizeDetails(IDetailLayoutBuilder& DetailLayout) override;
	//~ End IDetailCustomization Interface

private:
	TWeakObjectPtr<class URuleNodeBase> RuleNodePtr;
};
