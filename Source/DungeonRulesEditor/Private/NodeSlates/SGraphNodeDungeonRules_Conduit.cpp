// Copyright (c) 2024 Benoit Pelletier
// SPDX-License-Identifier: BSL-1.0
// Distributed under the Boost Software License, Version 1.0. 
// (See accompanying file LICENSE or copy at https://www.boost.org/LICENSE_1_0.txt)

#include "SGraphNodeDungeonRules_Conduit.h"
#include "Nodes/DungeonRulesNode_Conduit.h"
#include "IDocumentation.h"
#include "SGraphPanel.h"
#include "SGraphPin.h"
#include "Styling/SlateStyleRegistry.h"

class SWidget;
class UEdGraphSchema;
struct FGeometry;
struct FPointerEvent;
struct FSlateBrush;

#define LOCTEXT_NAMESPACE "SGraphNodeDungeonRules_State"

/////////////////////////////////////////////////////
// SStateMachineOutputPin

class SConduitOutputPin : public SGraphPin
{
public:
	SLATE_BEGIN_ARGS(SConduitOutputPin){}
	SLATE_END_ARGS()

	void Construct(const FArguments& InArgs, UEdGraphPin* InPin);
protected:
	// Begin SGraphPin interface
	virtual TSharedRef<SWidget>	GetDefaultValueWidget() override;
	virtual FSlateColor GetPinColor() const override;
	// End SGraphPin interface

	const FSlateBrush* GetPinBorder() const;
};

void SConduitOutputPin::Construct(const FArguments& InArgs, UEdGraphPin* InPin)
{
	this->SetCursor( EMouseCursor::Default );

	bShowLabel = true;

	GraphPinObj = InPin;
	check(GraphPinObj != NULL);

	const UEdGraphSchema* Schema = GraphPinObj->GetSchema();
	check(Schema);

	// Set up a hover for pins that is tinted the color of the pin.
	SBorder::Construct( SBorder::FArguments()
		.BorderImage( this, &SConduitOutputPin::GetPinBorder )
		.BorderBackgroundColor( this, &SConduitOutputPin::GetPinColor )
		.OnMouseButtonDown( this, &SConduitOutputPin::OnPinMouseDown )
		.Cursor( this, &SConduitOutputPin::GetPinCursor )
	);
}

TSharedRef<SWidget>	SConduitOutputPin::GetDefaultValueWidget()
{
	return SNew(STextBlock);
}

FSlateColor SConduitOutputPin::GetPinColor() const
{
	return FLinearColor(0.8f, 0.8f, 0.8f);
}

const FSlateBrush* SConduitOutputPin::GetPinBorder() const
{
	const ISlateStyle* DungeonRulesStyle = FSlateStyleRegistry::FindSlateStyle("DungeonRulesStyle");
	if (!DungeonRulesStyle)
	{
		UE_LOG(LogTemp, Error, TEXT("DungeonRulesStyle style set was not found!"));
	}

	return ( IsHovered() )
		? DungeonRulesStyle->GetBrush(TEXT("DungeonRules.ConduitNode.Pin.Hovered"))
		: FAppStyle::GetBrush(TEXT("NoBrush"));
	//return ( IsHovered() )
	//	? FAppStyle::GetBrush( TEXT("Graph.StateNode.Pin.BackgroundHovered") )
	//	: FAppStyle::GetBrush( TEXT("Graph.StateNode.Pin.Background") );
}

/////////////////////////////////////////////////////
// SGraphNodeDungeonRules_State

void SGraphNodeDungeonRules_Conduit::Construct(const FArguments& InArgs, UDungeonRulesNode* InNode)
{
	this->GraphNode = InNode;

	this->SetCursor(EMouseCursor::CardinalCross);

	this->UpdateGraphNode();
}

FSlateColor SGraphNodeDungeonRules_Conduit::GetBorderBackgroundColor() const
{
	FLinearColor InactiveStateColor(0.08f, 0.08f, 0.08f);
	FLinearColor ActiveStateColorDim(0.4f, 0.3f, 0.15f);
	FLinearColor ActiveStateColorBright(1.f, 0.6f, 0.35f);

	return GetBorderBackgroundColor_Internal(InactiveStateColor, ActiveStateColorDim, ActiveStateColorBright);
}

FSlateColor SGraphNodeDungeonRules_Conduit::GetBorderBackgroundColor_Internal(FLinearColor InactiveStateColor, FLinearColor ActiveStateColorDim, FLinearColor ActiveStateColorBright) const
{
	return InactiveStateColor;
}

void SGraphNodeDungeonRules_Conduit::OnMouseEnter(const FGeometry& MyGeometry, const FPointerEvent& MouseEvent)
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

void SGraphNodeDungeonRules_Conduit::OnMouseLeave(const FPointerEvent& MouseEvent)
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

const FSlateBrush* SGraphNodeDungeonRules_Conduit::GetShadowBrush(bool bSelected) const
{
	const ISlateStyle* DungeonRulesStyle = FSlateStyleRegistry::FindSlateStyle("DungeonRulesStyle");
	if (!DungeonRulesStyle)
	{
		UE_LOG(LogTemp, Error, TEXT("DungeonRulesStyle style set was not found!"));
	}

	return (bSelected)
		? DungeonRulesStyle->GetBrush(TEXT("DungeonRules.ConduitNode.ShadowSelected"))
		: DungeonRulesStyle->GetBrush(TEXT("DungeonRules.ConduitNode.Shadow"));
}

void SGraphNodeDungeonRules_Conduit::UpdateGraphNode()
{
	InputPins.Empty();
	OutputPins.Empty();
	
	// Reset variables that are going to be exposed, in case we are refreshing an already setup node.
	RightNodeBox.Reset();
	LeftNodeBox.Reset();

	const ISlateStyle* DungeonRulesStyle = FSlateStyleRegistry::FindSlateStyle("DungeonRulesStyle");
	if (!DungeonRulesStyle)
	{
		UE_LOG(LogTemp, Error, TEXT("DungeonRulesStyle style set was not found!"));
	}

	FSlateRenderTransform Rotated45Degrees(FQuat2f(FMath::DegreesToRadians(-45.0f)));
	FVector2D CenteredPivot = FVector2D(0.5, 0.5);

	this->ContentScale.Bind( this, &SGraphNode::GetContentScale );
	this->SetRenderTransform(Rotated45Degrees);
	this->SetRenderTransformPivot(CenteredPivot);
	this->GetOrAddSlot( ENodeZone::Center )
		.HAlign(HAlign_Center)
		.VAlign(VAlign_Center)
		[
			SNew(SBorder)
			.BorderImage(FAppStyle::GetBrush("Graph.StateNode.Body"))
			.Padding(0)
			.BorderBackgroundColor(this, &SGraphNodeDungeonRules_Conduit::GetBorderBackgroundColor)
			[
				SNew(SOverlay)

				// PIN AREA
				+ SOverlay::Slot()
				.HAlign(HAlign_Fill)
				.VAlign(VAlign_Fill)
				[
					SAssignNew(RightNodeBox, SVerticalBox)
				]

				// STATE NAME AREA
				+ SOverlay::Slot()
				.HAlign(HAlign_Center)
				.VAlign(VAlign_Center)
				.Padding(10.0f)
				[
					SNew(SImage)
					.Image(DungeonRulesStyle->GetBrush("DungeonRules.ConduitNode.Body"))
						.DesiredSizeOverride(FVector2D(20.0f, 20.0f))
				]
			]
		];

	CreatePinWidgets();
}

void SGraphNodeDungeonRules_Conduit::CreatePinWidgets()
{
	UDungeonRulesNode* StateNode = CastChecked<UDungeonRulesNode>(GraphNode);

	UEdGraphPin* CurPin = StateNode->GetOutputPin();
	if (!CurPin->bHidden)
	{
		TSharedPtr<SGraphPin> NewPin = SNew(SConduitOutputPin, CurPin);

		this->AddPin(NewPin.ToSharedRef());
	}
}

void SGraphNodeDungeonRules_Conduit::AddPin(const TSharedRef<SGraphPin>& PinToAdd)
{
	PinToAdd->SetOwner( SharedThis(this) );
	//RightNodeBox->SetRenderOpacity(0.2f);
	RightNodeBox->AddSlot()
		.HAlign(HAlign_Fill)
		.VAlign(VAlign_Fill)
		.FillHeight(1.0f)
		[
			PinToAdd
		];
	OutputPins.Add(PinToAdd);
}

TSharedPtr<SToolTip> SGraphNodeDungeonRules_Conduit::GetComplexTooltip()
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
				IDocumentation::Get()->CreateToolTip(FText::FromString("Documentation"), NULL, StateNode->GetDocumentationLink(), StateNode->GetDocumentationExcerptName())
			]
		];
}

#undef LOCTEXT_NAMESPACE
