// Copyright Epic Games, Inc. All Rights Reserved.

#include "DungeonDetailsCustomization.h"
#include "DetailCategoryBuilder.h"
#include "DetailLayoutBuilder.h"
#include "DetailWidgetRow.h"
#include "EdGraph/EdGraphNode.h"
#include "EdGraphNode_Comment.h"
#include "Widgets/Input/SEditableTextBox.h"
#include "Widgets/Input/SMultiLineEditableTextBox.h"
#include "DungeonRules.h"
#include "Nodes/RuleNodeBase.h"

#define LOCTEXT_NAMESPACE "DungeonDetailsCustomization"

namespace BlueprintDocumentationDetailDefs
{
	/** Minimum size of the details title panel */
	static const float DetailsTitleMinWidth = 125.f;
	/** Maximum size of the details title panel */
	static const float DetailsTitleMaxWidth = 300.f;
	/** magic number retrieved from SGraphNodeComment::GetWrapAt() */
	static const float DetailsTitleWrapPadding = 32.0f;
};

BEGIN_SLATE_FUNCTION_BUILD_OPTIMIZATION
void FEditorGraphNodeNameDetails::CustomizeDetails( IDetailLayoutBuilder& DetailLayout )
{
	const TArray<TWeakObjectPtr<UObject>>& SelectedObjects = DetailLayout.GetSelectedObjects();
	if( SelectedObjects.Num() == 1 )
	{
		if (SelectedObjects[0].IsValid() && SelectedObjects[0]->IsA<UEdGraphNode>())
		{
			GraphNodePtr = Cast<UEdGraphNode>(SelectedObjects[0].Get());
		}
	}

	if(!GraphNodePtr.IsValid() || !GraphNodePtr.Get()->GetCanRenameNode())
	{
		return;
	}

	IDetailCategoryBuilder& Category = DetailLayout.EditCategory("GraphNodeDetail", LOCTEXT("GraphNodeDetailsCategory", "Graph Node"), ECategoryPriority::Important);
	const FSlateFontInfo DetailFontInfo = IDetailLayoutBuilder::GetDetailFont();
	FText RowHeader;
	FText NameContent;

	if( GraphNodePtr->IsA( UEdGraphNode_Comment::StaticClass() ))
	{
		RowHeader = LOCTEXT("GraphNodeDetail_CommentRowTitle", "Comment");
		NameContent = LOCTEXT("GraphNodeDetail_CommentContentTitle", "Comment Text");
	}
	else
	{
		RowHeader = LOCTEXT("GraphNodeDetail_NodeRowTitle", "Node Title");
		NameContent = LOCTEXT("GraphNodeDetail_ContentTitle", "Name");
	}

	bool bNameAllowsMultiLine = false;
	if( GraphNodePtr.IsValid() && GraphNodePtr.Get()->IsA<UEdGraphNode_Comment>() )
	{
		bNameAllowsMultiLine = true;
	}

	TSharedPtr<SWidget> EditNameWidget;
	float WidgetMinDesiredWidth = BlueprintDocumentationDetailDefs::DetailsTitleMinWidth;
	float WidgetMaxDesiredWidth = BlueprintDocumentationDetailDefs::DetailsTitleMaxWidth;
	if( bNameAllowsMultiLine )
	{
		SAssignNew(MultiLineNameEditableTextBox, SMultiLineEditableTextBox)
		.Text(this, &FEditorGraphNodeNameDetails::OnGetName)
		.OnTextChanged(this, &FEditorGraphNodeNameDetails::OnNameChanged)
		.OnTextCommitted(this, &FEditorGraphNodeNameDetails::OnNameCommitted)
		.ClearKeyboardFocusOnCommit(true)
		.ModiferKeyForNewLine(EModifierKey::Shift)
		.RevertTextOnEscape(true)
		.SelectAllTextWhenFocused(true)
		.IsReadOnly(this, &FEditorGraphNodeNameDetails::IsNameReadOnly)
		.Font(DetailFontInfo)
		.WrapTextAt(WidgetMaxDesiredWidth - BlueprintDocumentationDetailDefs::DetailsTitleWrapPadding)
		;

		EditNameWidget = MultiLineNameEditableTextBox;
	}
	else
	{
		SAssignNew(NameEditableTextBox, SEditableTextBox)
		.Text(this, &FEditorGraphNodeNameDetails::OnGetName)
		.OnTextChanged(this, &FEditorGraphNodeNameDetails::OnNameChanged)
		.OnTextCommitted(this, &FEditorGraphNodeNameDetails::OnNameCommitted)
		.IsReadOnly(this, &FEditorGraphNodeNameDetails::IsNameReadOnly)
		.Font(DetailFontInfo);

		EditNameWidget = NameEditableTextBox;
		WidgetMaxDesiredWidth = WidgetMinDesiredWidth;
	}

	Category.AddCustomRow( RowHeader )
	.NameContent()
	[
		SNew(STextBlock)
		.Text( NameContent )
		.Font(DetailFontInfo)
	]
	.ValueContent()
	.MinDesiredWidth(WidgetMinDesiredWidth)
	.MaxDesiredWidth(WidgetMaxDesiredWidth)
	[
		EditNameWidget.ToSharedRef()
	];
}
END_SLATE_FUNCTION_BUILD_OPTIMIZATION

void FEditorGraphNodeNameDetails::SetNameError( const FText& Error )
{
	if( NameEditableTextBox.IsValid() )
	{
		NameEditableTextBox->SetError( Error );
	}
	if( MultiLineNameEditableTextBox.IsValid() )
	{
		MultiLineNameEditableTextBox->SetError( Error );
	}
}

bool FEditorGraphNodeNameDetails::IsNameReadOnly() const
{
	bool bReadOnly = true;
	if(GraphNodePtr.IsValid())
	{
		bReadOnly = !GraphNodePtr->GetCanRenameNode();
	}
	return bReadOnly;
}

FText FEditorGraphNodeNameDetails::OnGetName() const
{
	FText Name;
	if(GraphNodePtr.IsValid())
	{
		Name = GraphNodePtr->GetNodeTitle( ENodeTitleType::EditableTitle );
	}
	return Name;
}

struct FGraphNodeNameValidatorHelper
{
	static EValidatorResult Validate(TWeakObjectPtr<UEdGraphNode> GraphNodePtr, const FString& NewName)
	{
		check(GraphNodePtr.IsValid());
		TSharedPtr<INameValidatorInterface> NameValidator = GraphNodePtr->MakeNameValidator();
		return (NameValidator.IsValid()) ? NameValidator->IsValid(NewName) : EValidatorResult::Ok;
	}
};

void FEditorGraphNodeNameDetails::OnNameChanged(const FText& InNewText)
{
	if( GraphNodePtr.IsValid())
	{
		const EValidatorResult ValidatorResult = FGraphNodeNameValidatorHelper::Validate(GraphNodePtr, InNewText.ToString());
		if(ValidatorResult == EValidatorResult::AlreadyInUse)
		{
			SetNameError(FText::Format(LOCTEXT("RenameFailed_InUse", "{0} is in use by another variable or function!"), InNewText));
		}
		else if(ValidatorResult == EValidatorResult::EmptyName)
		{
			SetNameError(LOCTEXT("RenameFailed_LeftBlank", "Names cannot be left blank!"));
		}
		else if(ValidatorResult == EValidatorResult::TooLong)
		{
			SetNameError(FText::Format( LOCTEXT("RenameFailed_NameTooLong", "Names must have fewer than {0} characters!"), FText::AsNumber( FKismetNameValidator::GetMaximumNameLength())));
		}
		else
		{
			SetNameError(FText::GetEmpty());
		}
	}
}

void FEditorGraphNodeNameDetails::OnNameCommitted(const FText& InNewText, ETextCommit::Type InTextCommit)
{
	if (ToolkitPtr && GraphNodePtr.IsValid())
	{
		if (FGraphNodeNameValidatorHelper::Validate(GraphNodePtr, InNewText.ToString()) == EValidatorResult::Ok)
		{
			ToolkitPtr->OnNodeTitleCommitted(InNewText, InTextCommit, GraphNodePtr.Get());
		}
	}
}

/////////////////////////////////////////////////////////////////////////////

BEGIN_SLATE_FUNCTION_BUILD_OPTIMIZATION
void FDungeonRuleNodeBaseDetails::CustomizeDetails(IDetailLayoutBuilder& DetailLayout)
{
	const TArray<TWeakObjectPtr<UObject>>& SelectedObjects = DetailLayout.GetSelectedObjects();
	if (SelectedObjects.Num() == 1)
	{
		if (SelectedObjects[0].IsValid() && SelectedObjects[0]->IsA<UEdGraphNode>())
		{
			RuleNodePtr = Cast<URuleNodeBase>(SelectedObjects[0].Get());
		}
	}

	if (!RuleNodePtr.IsValid() || !RuleNodePtr->GetNodeInstance())
		return;

	for (const FName& PropertyName : RuleNodePtr->GetPropertyNamesToEdit())
	{
		const TSharedPtr<IPropertyHandle> PropertyHandle = DetailLayout.AddObjectPropertyData({RuleNodePtr->GetNodeInstance()}, PropertyName);
		if (PropertyHandle.IsValid())
			DetailLayout.AddPropertyToCategory(PropertyHandle);
	}
}
END_SLATE_FUNCTION_BUILD_OPTIMIZATION

#undef LOCTEXT_NAMESPACE
