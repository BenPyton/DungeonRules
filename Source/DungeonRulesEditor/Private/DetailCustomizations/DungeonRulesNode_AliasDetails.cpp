// Copyright Epic Games, Inc. All Rights Reserved.

#include "DungeonRulesNode_AliasDetails.h"
#include "Nodes/DungeonRulesNode_Alias.h"
#include "Nodes/DungeonRulesNode_State.h"
#include "DungeonRulesGraph.h"
#include "DetailLayoutBuilder.h"
#include "DetailWidgetRow.h"

#define LOCTEXT_NAMESPACE "FDungeonRulesNode_AliasDetails"

/////////////////////////////////////////////////////////////////////////

TSharedRef<IDetailCustomization> FDungeonRulesNode_AliasDetails::MakeInstance()
{
	return MakeShareable( new FDungeonRulesNode_AliasDetails);
}

void FDungeonRulesNode_AliasDetails::CustomizeDetails( IDetailLayoutBuilder& DetailBuilder )
{
	// Get a handle to the node we're viewing
	const TArray< TWeakObjectPtr<UObject> >& SelectedObjects = DetailBuilder.GetSelectedObjects();
	if (SelectedObjects.Num() != 1)
	{
		return;
	}

	const TWeakObjectPtr<UObject>& CurrentObject = SelectedObjects[0];
	UDungeonRulesNode_Alias* StateAliasNode = nullptr;
	if (CurrentObject.IsValid())
	{
		StateAliasNodeWeak = StateAliasNode = Cast<UDungeonRulesNode_Alias>(CurrentObject.Get());
	}

	if (StateAliasNode == nullptr)
	{
		return;
	}

	GenerateStatePickerDetails(*StateAliasNode, DetailBuilder);
}

void FDungeonRulesNode_AliasDetails::GetReferenceableStates(const UDungeonRulesNode_Alias& OwningNode, TSet<TWeakObjectPtr<UDungeonRulesNode>>& OutStates) const
{
	UDungeonRulesGraph* DungeonRules = CastChecked<UDungeonRulesGraph>(OwningNode.GetOuter());

	TArray<UDungeonRulesNode_State*> Nodes;
	DungeonRules->GetNodesOfClass<UDungeonRulesNode_State>(Nodes);
	for (auto NodeIt = Nodes.CreateIterator(); NodeIt; ++NodeIt)
	{
		auto Node = *NodeIt;
		OutStates.Add(Node);
	}
}

bool FDungeonRulesNode_AliasDetails::IsGlobalAlias() const
{
	if (UDungeonRulesNode_Alias* StateAliasNode = StateAliasNodeWeak.Get())
	{
		return StateAliasNode->bGlobalAlias;
	}
	
	return false;
}

void FDungeonRulesNode_AliasDetails::OnPropertyAliasAllStatesCheckboxChanged(ECheckBoxState NewState)
{
	if (UDungeonRulesNode_Alias* StateAliasNode = StateAliasNodeWeak.Get())
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
	}
}

ECheckBoxState FDungeonRulesNode_AliasDetails::AreAllStatesAliased() const
{
	if (const UDungeonRulesNode_Alias* StateAliasNode = StateAliasNodeWeak.Get())
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

void FDungeonRulesNode_AliasDetails::OnPropertyIsStateAliasedCheckboxChanged(ECheckBoxState NewState, const TWeakObjectPtr<UDungeonRulesNode> StateNodeWeak)
{
	if (!StateNodeWeak.IsValid())
		return;

	if (UDungeonRulesNode_Alias* StateAliasNode = StateAliasNodeWeak.Get())
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
	}
}

ECheckBoxState FDungeonRulesNode_AliasDetails::IsStateAliased(const TWeakObjectPtr<UDungeonRulesNode> StateNodeWeak) const
{
	if (const UDungeonRulesNode* StateNode = StateNodeWeak.Get())
	{
		if (const UDungeonRulesNode_Alias* StateAliasNode = StateAliasNodeWeak.Get())
		{
			if (StateAliasNode->GetAliasedStates().Find(StateNodeWeak))
			{
				return ECheckBoxState::Checked;
			}
		}
	}

	return ECheckBoxState::Unchecked;
}

void FDungeonRulesNode_AliasDetails::GenerateStatePickerDetails(UDungeonRulesNode_Alias& AliasNode, IDetailLayoutBuilder& DetailBuilder)
{
	ReferenceableStates.Reset();
	GetReferenceableStates(AliasNode, ReferenceableStates);

	if (ReferenceableStates.Num() > 0)
	{
		IDetailCategoryBuilder& CategoryBuilder = DetailBuilder.EditCategory(FName(TEXT("Rule Alias")));
		CategoryBuilder.AddProperty(GET_MEMBER_NAME_CHECKED(UDungeonRulesNode_Alias, bGlobalAlias));

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
					.IsChecked(this, &FDungeonRulesNode_AliasDetails::AreAllStatesAliased)
					.OnCheckStateChanged(this, &FDungeonRulesNode_AliasDetails::OnPropertyAliasAllStatesCheckboxChanged)
					.IsEnabled_Lambda([this]() -> bool 
						{
							return !IsGlobalAlias();
						})
				]
			];

		for (auto StateIt = ReferenceableStates.CreateConstIterator(); StateIt; ++StateIt)
		{
			const TWeakObjectPtr<UDungeonRulesNode>& StateNodeWeak = *StateIt;
			if (const UDungeonRulesNode* StateNode = StateNodeWeak.Get())
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
							.IsChecked(this, &FDungeonRulesNode_AliasDetails::IsStateAliased, StateNodeWeak)
							.OnCheckStateChanged(this, &FDungeonRulesNode_AliasDetails::OnPropertyIsStateAliasedCheckboxChanged, StateNodeWeak)
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
