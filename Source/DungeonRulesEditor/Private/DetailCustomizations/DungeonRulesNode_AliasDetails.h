// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "Containers/Set.h"
#include "IDetailCustomization.h"

class IDetailLayoutBuilder;
class UDungeonRulesNode_Alias;
class UDungeonRulesNode;

class FDungeonRulesNode_AliasDetails : public IDetailCustomization
{
public:
	/** Makes a new instance of this detail layout class for a specific detail view requesting it */
	static TSharedRef<IDetailCustomization> MakeInstance();

	//~ Begin IDetailCustomization Interface
	virtual void CustomizeDetails( IDetailLayoutBuilder& DetailBuilder ) override;
	//~ End IDetailCustomization Interface

private:

	void GetReferenceableStates(const UDungeonRulesNode_Alias& OwningNode, TSet<TWeakObjectPtr<UDungeonRulesNode>>& OutStates) const;

	bool IsGlobalAlias() const;
	void OnPropertyAliasAllStatesCheckboxChanged(ECheckBoxState NewState);
	ECheckBoxState AreAllStatesAliased() const;
	void OnPropertyIsStateAliasedCheckboxChanged(ECheckBoxState NewState, const TWeakObjectPtr<UDungeonRulesNode> StateNodeWeak);
	ECheckBoxState IsStateAliased(const TWeakObjectPtr<UDungeonRulesNode> StateNodeWeak) const;

	void GenerateStatePickerDetails(UDungeonRulesNode_Alias& AliasNode, IDetailLayoutBuilder& DetailBuilder);

	TWeakObjectPtr<UDungeonRulesNode_Alias> StateAliasNodeWeak;
	TSet<TWeakObjectPtr<UDungeonRulesNode>> ReferenceableStates;
};

