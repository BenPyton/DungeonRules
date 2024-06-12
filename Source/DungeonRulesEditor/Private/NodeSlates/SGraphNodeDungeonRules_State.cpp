// Copyright (c) 2024 Benoit Pelletier
// SPDX-License-Identifier: BSL-1.0
// Distributed under the Boost Software License, Version 1.0. 
// (See accompanying file LICENSE or copy at https://www.boost.org/LICENSE_1_0.txt)

#include "SGraphNodeDungeonRules_State.h"
#include "Nodes/DungeonRulesNode_Conduit.h"
#include "Nodes/DungeonRulesNode.h"
#include "IDocumentation.h"
#include "SGraphPanel.h"
#include "SGraphPin.h"
#include "Widgets/Text/SInlineEditableTextBlock.h"

class SWidget;
class UEdGraphSchema;
struct FGeometry;
struct FPointerEvent;
struct FSlateBrush;

#define LOCTEXT_NAMESPACE "SGraphNodeDungeonRules_State"

/////////////////////////////////////////////////////
// SStateMachineOutputPin

class SStateMachineOutputPin : public SGraphPin
{
public:
	SLATE_BEGIN_ARGS(SStateMachineOutputPin){}
	SLATE_END_ARGS()

	void Construct(const FArguments& InArgs, UEdGraphPin* InPin);
protected:
	// Begin SGraphPin interface
	virtual TSharedRef<SWidget>	GetDefaultValueWidget() override;
	// End SGraphPin interface

	const FSlateBrush* GetPinBorder() const;
};

void SStateMachineOutputPin::Construct(const FArguments& InArgs, UEdGraphPin* InPin)
{
	this->SetCursor( EMouseCursor::Default );

	bShowLabel = true;

	GraphPinObj = InPin;
	check(GraphPinObj != NULL);

	const UEdGraphSchema* Schema = GraphPinObj->GetSchema();
	check(Schema);

	// Set up a hover for pins that is tinted the color of the pin.
	SBorder::Construct( SBorder::FArguments()
		.BorderImage( this, &SStateMachineOutputPin::GetPinBorder )
		.BorderBackgroundColor( this, &SStateMachineOutputPin::GetPinColor )
		.OnMouseButtonDown( this, &SStateMachineOutputPin::OnPinMouseDown )
		.Cursor( this, &SStateMachineOutputPin::GetPinCursor )
	);
}

TSharedRef<SWidget>	SStateMachineOutputPin::GetDefaultValueWidget()
{
	return SNew(STextBlock);
}

const FSlateBrush* SStateMachineOutputPin::GetPinBorder() const
{
	return ( IsHovered() )
		? FAppStyle::GetBrush( TEXT("Graph.StateNode.Pin.BackgroundHovered") )
		: FAppStyle::GetBrush( TEXT("Graph.StateNode.Pin.Background") );
}

/////////////////////////////////////////////////////
// SGraphNodeDungeonRules_State

void SGraphNodeDungeonRules_State::Construct(const FArguments& InArgs, UDungeonRulesNode* InNode)
{
	this->GraphNode = InNode;

	this->SetCursor(EMouseCursor::CardinalCross);

	this->UpdateGraphNode();
}

FSlateColor SGraphNodeDungeonRules_State::GetBorderBackgroundColor() const
{
	FLinearColor InactiveStateColor(0.08f, 0.08f, 0.08f);
	FLinearColor ActiveStateColorDim(0.4f, 0.3f, 0.15f);
	FLinearColor ActiveStateColorBright(1.f, 0.6f, 0.35f);

	return GetBorderBackgroundColor_Internal(InactiveStateColor, ActiveStateColorDim, ActiveStateColorBright);
}

FSlateColor SGraphNodeDungeonRules_State::GetBorderBackgroundColor_Internal(FLinearColor InactiveStateColor, FLinearColor ActiveStateColorDim, FLinearColor ActiveStateColorBright) const
{
	return InactiveStateColor;
}

void SGraphNodeDungeonRules_State::OnMouseEnter(const FGeometry& MyGeometry, const FPointerEvent& MouseEvent)
{
	// Add pins to the hover set so outgoing transitions arrows remains highlighted while the mouse is over the state node
	if (const UDungeonRulesNode* StateNode = Cast<UDungeonRulesNode>(GraphNode))
	{
		if (const UEdGraphPin* OutputPin = StateNode->GetOutputPin())
		{
			TSharedPtr<SGraphPanel> OwnerPanel = GetOwnerPanel();
			check(OwnerPanel.IsValid());

			for (int32 LinkIndex = 0; LinkIndex < OutputPin->LinkedTo.Num(); ++LinkIndex)
			{
				OwnerPanel->AddPinToHoverSet(OutputPin->LinkedTo[LinkIndex]);
			}
		}
	}
	
	SGraphNode::OnMouseEnter(MyGeometry, MouseEvent);
}

void SGraphNodeDungeonRules_State::OnMouseLeave(const FPointerEvent& MouseEvent)
{
	// Remove manually added pins from the hover set
	if (const UDungeonRulesNode* StateNode = Cast<UDungeonRulesNode>(GraphNode))
	{
		if(const UEdGraphPin* OutputPin = StateNode->GetOutputPin())
		{
			TSharedPtr<SGraphPanel> OwnerPanel = GetOwnerPanel();
			check(OwnerPanel.IsValid());

			for (int32 LinkIndex = 0; LinkIndex < OutputPin->LinkedTo.Num(); ++LinkIndex)
			{
				OwnerPanel->RemovePinFromHoverSet(OutputPin->LinkedTo[LinkIndex]);
			}
		}
	}

	SGraphNode::OnMouseLeave(MouseEvent);
}

void SGraphNodeDungeonRules_State::UpdateGraphNode()
{
	InputPins.Empty();
	OutputPins.Empty();
	
	// Reset variables that are going to be exposed, in case we are refreshing an already setup node.
	RightNodeBox.Reset();
	LeftNodeBox.Reset();

	const FSlateBrush* NodeTypeIcon = GetNameIcon();

	FLinearColor TitleShadowColor(0.6f, 0.6f, 0.6f);
	TSharedPtr<SErrorText> ErrorText;
	NodeTitle = SNew(SNodeTitle, GraphNode);

	this->ContentScale.Bind( this, &SGraphNode::GetContentScale );
	this->GetOrAddSlot( ENodeZone::Center )
		.HAlign(HAlign_Center)
		.VAlign(VAlign_Center)
		[
			SNew(SBorder)
			.BorderImage( FAppStyle::GetBrush( "Graph.StateNode.Body" ) )
			.Padding(0)
			.BorderBackgroundColor( this, &SGraphNodeDungeonRules_State::GetBorderBackgroundColor )
			[
				SNew(SOverlay)

				// PIN AREA
				+SOverlay::Slot()
				.HAlign(HAlign_Fill)
				.VAlign(VAlign_Fill)
				[
					SAssignNew(RightNodeBox, SVerticalBox)
				]

				// STATE NAME AREA
				+SOverlay::Slot()
				.HAlign(HAlign_Center)
				.VAlign(VAlign_Center)
				.Padding(10.0f)
				[
					SNew(SBorder)
					.BorderImage( FAppStyle::GetBrush("Graph.StateNode.ColorSpill") )
					.BorderBackgroundColor( TitleShadowColor )
					.HAlign(HAlign_Center)
					.VAlign(VAlign_Center)
					.Visibility(EVisibility::SelfHitTestInvisible)
					[
						SNew(SHorizontalBox)
						+SHorizontalBox::Slot()
						.AutoWidth()
						[
							// POPUP ERROR MESSAGE
							SAssignNew(ErrorText, SErrorText )
							.BackgroundColor( this, &SGraphNodeDungeonRules_State::GetErrorColor )
							.ToolTipText( this, &SGraphNodeDungeonRules_State::GetErrorMsgToolTip )
						]
#if false // TODO: should be kept or removed?
						+SHorizontalBox::Slot()
						.AutoWidth()
						.VAlign(VAlign_Center)
						[
							SNew(SImage)
							.Image(NodeTypeIcon)
						]
#endif
						+SHorizontalBox::Slot()
						.Padding(FMargin(4.0f, 0.0f, 4.0f, 0.0f))
						[
							SNew(SVerticalBox)
							+SVerticalBox::Slot()
								.AutoHeight()
							[
								SAssignNew(InlineEditableText, SInlineEditableTextBlock)
								.Style(FAppStyle::Get(), "Graph.StateNode.NodeTitleInlineEditableText")
								.Text(this, &SGraphNodeDungeonRules_State::GetNodeName)
								.OnVerifyTextChanged(this, &SGraphNodeDungeonRules_State::OnVerifyNameTextChanged)
								.OnTextCommitted(this, &SGraphNodeDungeonRules_State::OnNameTextCommited)
								.IsReadOnly(this, &SGraphNodeDungeonRules_State::IsNameReadOnly)
								.IsSelected(this, &SGraphNodeDungeonRules_State::IsSelectedExclusively)
							]
							+SVerticalBox::Slot()
								.AutoHeight()
							[
								NodeTitle.ToSharedRef()
							]
						]
					]
				]
			]
		];

	ErrorReporting = ErrorText;
	ErrorReporting->SetError(ErrorMsg);
	CreatePinWidgets();
}

void SGraphNodeDungeonRules_State::CreatePinWidgets()
{
	UDungeonRulesNode* StateNode = CastChecked<UDungeonRulesNode>(GraphNode);

	UEdGraphPin* CurPin = StateNode->GetOutputPin();
	if (!CurPin->bHidden)
	{
		TSharedPtr<SGraphPin> NewPin = SNew(SStateMachineOutputPin, CurPin);

		this->AddPin(NewPin.ToSharedRef());
	}
}

void SGraphNodeDungeonRules_State::AddPin(const TSharedRef<SGraphPin>& PinToAdd)
{
	PinToAdd->SetOwner( SharedThis(this) );
	RightNodeBox->AddSlot()
		.HAlign(HAlign_Fill)
		.VAlign(VAlign_Fill)
		.FillHeight(1.0f)
		[
			PinToAdd
		];
	OutputPins.Add(PinToAdd);
}

TSharedPtr<SToolTip> SGraphNodeDungeonRules_State::GetComplexTooltip()
{
	UDungeonRulesNode* StateNode = CastChecked<UDungeonRulesNode>(GraphNode);
	FText TooltipDesc = StateNode->GetTooltipText();

	return SNew(SToolTip)
		[
			SNew(SVerticalBox)
			+SVerticalBox::Slot()
			.AutoHeight()
			.Padding(2.0f)
			[
				SNew(STextBlock)
					.TextStyle(FAppStyle::Get(), TEXT("Graph.TransitionNode.TooltipName"))
					.Text(TooltipDesc)
			]
			+SVerticalBox::Slot()
			.AutoHeight()
			.Padding(FMargin(0.0f, 5.0f, 0.0f, 0.0f))
			[
				IDocumentation::Get()->CreateToolTip(FText::FromString("Documentation"), nullptr, StateNode->GetDocumentationLink(), StateNode->GetDocumentationExcerptName())
			]
		];
}

FText SGraphNodeDungeonRules_State::GetPreviewCornerText() const
{
	UDungeonRulesNode* StateNode = CastChecked<UDungeonRulesNode>(GraphNode);

	return FText::Format(NSLOCTEXT("SGraphNodeDungeonRules_State", "PreviewCornerStateText", "{0} state"), FText::FromString(StateNode->GetStateName()));
}

const FSlateBrush* SGraphNodeDungeonRules_State::GetNameIcon() const
{
	return FAppStyle::GetBrush( TEXT("Graph.StateNode.Icon") );
}

FText SGraphNodeDungeonRules_State::GetNodeName() const
{
	return NodeTitle.IsValid() ? NodeTitle->GetHeadTitle() : FText::FromString(TEXT("NULL"));
}

#undef LOCTEXT_NAMESPACE
