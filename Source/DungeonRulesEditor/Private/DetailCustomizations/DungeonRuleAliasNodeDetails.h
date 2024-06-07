// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "Containers/Set.h"
#include "IDetailCustomization.h"

class IDetailLayoutBuilder;
class URuleAliasNode;
class URuleNodeBase;

class FDungeonRuleAliasNodeDetails : public IDetailCustomization
{
public:
	/** Makes a new instance of this detail layout class for a specific detail view requesting it */
	static TSharedRef<IDetailCustomization> MakeInstance();

	//~ Begin IDetailCustomization Interface
	virtual void CustomizeDetails( IDetailLayoutBuilder& DetailBuilder ) override;
	//~ End IDetailCustomization Interface

private:

	void GetReferenceableStates(const URuleAliasNode& OwningNode, TSet<TWeakObjectPtr<URuleNodeBase>>& OutStates) const;

	bool IsGlobalAlias() const;
	void OnPropertyAliasAllStatesCheckboxChanged(ECheckBoxState NewState);
	ECheckBoxState AreAllStatesAliased() const;
	void OnPropertyIsStateAliasedCheckboxChanged(ECheckBoxState NewState, const TWeakObjectPtr<URuleNodeBase> StateNodeWeak);
	ECheckBoxState IsStateAliased(const TWeakObjectPtr<URuleNodeBase> StateNodeWeak) const;

	void GenerateStatePickerDetails(URuleAliasNode& AliasNode, IDetailLayoutBuilder& DetailBuilder);

	TWeakObjectPtr<URuleAliasNode> StateAliasNodeWeak;
	TSet<TWeakObjectPtr<URuleNodeBase>> ReferenceableStates;
};

