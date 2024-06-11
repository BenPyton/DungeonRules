// Copyright Epic Games, Inc. All Rights Reserved.

#include "DungeonRuleAliasNodeDetails.h"
#include "Nodes/RuleAliasNode.h"
#include "Nodes/RuleNode.h"
#include "DungeonRulesGraph.h"
#include "DetailLayoutBuilder.h"
#include "DetailWidgetRow.h"

#define LOCTEXT_NAMESPACE "FDungeonRuleAliasNodeDetails"

/////////////////////////////////////////////////////////////////////////

TSharedRef<IDetailCustomization> FDungeonRuleAliasNodeDetails::MakeInstance()
{
	return MakeShareable( new FDungeonRuleAliasNodeDetails);
}

void FDungeonRuleAliasNodeDetails::CustomizeDetails( IDetailLayoutBuilder& DetailBuilder )
{
	// Get a handle to the node we're viewing
	const TArray< TWeakObjectPtr<UObject> >& SelectedObjects = DetailBuilder.GetSelectedObjects();
	if (SelectedObjects.Num() != 1)
	{
		return;
	}

	const TWeakObjectPtr<UObject>& CurrentObject = SelectedObjects[0];
	URuleAliasNode* StateAliasNode = nullptr;
	if (CurrentObject.IsValid())
	{
		StateAliasNodeWeak = StateAliasNode = Cast<URuleAliasNode>(CurrentObject.Get());
	}

	if (StateAliasNode == nullptr)
	{
		return;
	}

	GenerateStatePickerDetails(*StateAliasNode, DetailBuilder);
}

void FDungeonRuleAliasNodeDetails::GetReferenceableStates(const URuleAliasNode& OwningNode, TSet<TWeakObjectPtr<URuleNodeBase>>& OutStates) const
{
	UDungeonRulesGraph* DungeonRules = CastChecked<UDungeonRulesGraph>(OwningNode.GetOuter());

	TArray<URuleNode*> Nodes;
	DungeonRules->GetNodesOfClass<URuleNode>(Nodes);
	for (auto NodeIt = Nodes.CreateIterator(); NodeIt; ++NodeIt)
	{
		auto Node = *NodeIt;
		OutStates.Add(Node);
	}
}

bool FDungeonRuleAliasNodeDetails::IsGlobalAlias() const
{
	if (URuleAliasNode* StateAliasNode = StateAliasNodeWeak.Get())
	{
		return StateAliasNode->bGlobalAlias;
	}
	
	return false;
}

void FDungeonRuleAliasNodeDetails::OnPropertyAliasAllStatesCheckboxChanged(ECheckBoxState NewState)
{
	if (URuleAliasNode* StateAliasNode = StateAliasNodeWeak.Get())
	{
		FScopedTransaction Transaction(LOCTEXT("Undo_SelectAllAliasState", "Select all state alias"));

		StateAliasNode->Modify();
		if (NewState == ECheckBoxState::Checked)
		{
			StateAliasNode->GetAliasedStates() = ReferenceableStates;
		}
		else
		{
			StateAliasNode->GetAliasedStates().Reset();
		}

#if false // Blueprint
		if (UBlueprint* Blueprint = FBlueprintEditorUtils::FindBlueprintForNodeChecked(StateAliasNode))
		{
			FBlueprintEditorUtils::MarkBlueprintAsModified(Blueprint);
		}
#endif
	}
}

ECheckBoxState FDungeonRuleAliasNodeDetails::AreAllStatesAliased() const
{
	if (const URuleAliasNode* StateAliasNode = StateAliasNodeWeak.Get())
	{
		const int32 NumAlisedStates = StateAliasNode->GetAliasedStates().Num();
		const int32 NumStates = ReferenceableStates.Num();

		if (NumAlisedStates == 0)
		{
			return ECheckBoxState::Unchecked;
		}
		else if(NumAlisedStates == NumStates)
		{
			return ECheckBoxState::Checked;
		}
	}

	return ECheckBoxState::Undetermined;
}

void FDungeonRuleAliasNodeDetails::OnPropertyIsStateAliasedCheckboxChanged(ECheckBoxState NewState, const TWeakObjectPtr<URuleNodeBase> StateNodeWeak)
{
	if (!StateNodeWeak.IsValid())
		return;

	if (URuleAliasNode* StateAliasNode = StateAliasNodeWeak.Get())
	{
		FScopedTransaction Transaction(LOCTEXT("Undo_AliasState", "Select state alias"));

		StateAliasNode->Modify();
		if (NewState == ECheckBoxState::Checked)
		{
			StateAliasNode->GetAliasedStates().FindOrAdd(StateNodeWeak);
		}
		else
		{
			StateAliasNode->GetAliasedStates().Remove(StateNodeWeak);
		}

#if false // Blueprint
		if (UBlueprint* Blueprint = FBlueprintEditorUtils::FindBlueprintForNodeChecked(StateAliasNode))
		{
			FBlueprintEditorUtils::MarkBlueprintAsModified(Blueprint);
		}
#endif
	}
}

ECheckBoxState FDungeonRuleAliasNodeDetails::IsStateAliased(const TWeakObjectPtr<URuleNodeBase> StateNodeWeak) const
{
	if (const URuleNodeBase* StateNode = StateNodeWeak.Get())
	{
		if (const URuleAliasNode* StateAliasNode = StateAliasNodeWeak.Get())
		{
			if (StateAliasNode->GetAliasedStates().Find(StateNodeWeak))
			{
				return ECheckBoxState::Checked;
			}
		}
	}

	return ECheckBoxState::Unchecked;
}

void FDungeonRuleAliasNodeDetails::GenerateStatePickerDetails(URuleAliasNode& AliasNode, IDetailLayoutBuilder& DetailBuilder)
{
	ReferenceableStates.Reset();
	GetReferenceableStates(AliasNode, ReferenceableStates);

	if (ReferenceableStates.Num() > 0)
	{
		IDetailCategoryBuilder& CategoryBuilder = DetailBuilder.EditCategory(FName(TEXT("Rule Alias")));
		CategoryBuilder.AddProperty(GET_MEMBER_NAME_CHECKED(URuleAliasNode, bGlobalAlias));

		FDetailWidgetRow& HeaderWidgetRow = CategoryBuilder.AddCustomRow(LOCTEXT("SelectAll", "Select All"));

		HeaderWidgetRow.NameContent()
			[
				SNew(STextBlock)
				.Text(LOCTEXT("StateName", "Name"))
				.Font(IDetailLayoutBuilder::GetDetailFontBold())
			];

		HeaderWidgetRow.ValueContent()
			[
				SNew(SHorizontalBox)
				+ SHorizontalBox::Slot()
				.AutoWidth()
				.VAlign(VAlign_Center)
				[
					SNew(STextBlock)
					.Text(LOCTEXT("SelectAllStatesPropertyValue", "Select All"))
					.Font(IDetailLayoutBuilder::GetDetailFontBold())
				]
				+ SHorizontalBox::Slot()
				.FillWidth(1.0f)
				.HAlign(HAlign_Right)
				.VAlign(VAlign_Center)
				[
					SNew(SCheckBox)
					.IsChecked(this, &FDungeonRuleAliasNodeDetails::AreAllStatesAliased)
					.OnCheckStateChanged(this, &FDungeonRuleAliasNodeDetails::OnPropertyAliasAllStatesCheckboxChanged)
					.IsEnabled_Lambda([this]() -> bool 
						{
							return !IsGlobalAlias();
						})
				]
			];

		for (auto StateIt = ReferenceableStates.CreateConstIterator(); StateIt; ++StateIt)
		{
			const TWeakObjectPtr<URuleNodeBase>& StateNodeWeak = *StateIt;
			if (const URuleNodeBase* StateNode = StateNodeWeak.Get())
			{
				FString StateName = StateNode->GetStateName();
				FText StateText = FText::FromString(StateName);

				FDetailWidgetRow& PropertyWidgetRow = CategoryBuilder.AddCustomRow(StateText);

				PropertyWidgetRow.NameContent()
					[
						SNew(STextBlock)
						.Text(StateText)
						.ToolTipText(StateText)
						.Font(IDetailLayoutBuilder::GetDetailFont())
					];

				PropertyWidgetRow.ValueContent()
					[
						SNew(SHorizontalBox)
						+ SHorizontalBox::Slot()
						.FillWidth(1.0f)
						.HAlign(HAlign_Right)
						.VAlign(VAlign_Center)
						[
							SNew(SCheckBox)
							.IsChecked(this, &FDungeonRuleAliasNodeDetails::IsStateAliased, StateNodeWeak)
							.OnCheckStateChanged(this, &FDungeonRuleAliasNodeDetails::OnPropertyIsStateAliasedCheckboxChanged, StateNodeWeak)
							.IsEnabled_Lambda([this]() -> bool 
								{
								return !IsGlobalAlias();
								})
						]
					];
			}
		}
	}
}

#undef LOCTEXT_NAMESPACE
