// Copyright (c) 2024 Benoit Pelletier
// SPDX-License-Identifier: BSL-1.0
// Distributed under the Boost Software License, Version 1.0. 
// (See accompanying file LICENSE or copy at https://www.boost.org/LICENSE_1_0.txt)


#include "SGraphNodeDungeonRules_Transition.h"
#include "Nodes/DungeonRulesNode.h"
#include "Nodes/DungeonRulesNode_Transition.h"
#include "DungeonRules.h"
#include "ConnectionDrawingPolicy.h"
#include "Layout/Geometry.h"
#include "IDocumentation.h"
#include "SGraphPanel.h"

class SWidget;
class UEdGraphPin;
class UObject;
struct FPointerEvent;
struct FSlateBrush;

#define LOCTEXT_NAMESPACE "TransitionNodes"

/////////////////////////////////////////////////////
// SGraphNodeDungeonRules_Transition

void SGraphNodeDungeonRules_Transition::Construct(const FArguments& InArgs, UDungeonRulesNode_Transition* InNode)
{
	this->GraphNode = InNode;
	this->UpdateGraphNode();
}

void SGraphNodeDungeonRules_Transition::MoveTo(const FVector2D& NewPosition, FNodeSet& NodeFilter, bool bMarkDirty)
{
	// Ignored; position is set by the location of the attached state nodes
}

bool SGraphNodeDungeonRules_Transition::RequiresSecondPassLayout() const
{
	return true;
}

void SGraphNodeDungeonRules_Transition::PerformSecondPassLayout(const TMap< UObject*, TSharedRef<SNode> >& NodeToWidgetLookup) const
{
	UDungeonRulesNode_Transition* TransNode = CastChecked<UDungeonRulesNode_Transition>(GraphNode);

	// Find the geometry of the state nodes we're connecting
	FGeometry StartGeom;
	FGeometry EndGeom;

	int32 TransIndex = 0;
	int32 NumOfTrans = 1;

	UDungeonRulesNode* PrevState = TransNode->GetPreviousState();
	UDungeonRulesNode* NextState = TransNode->GetNextState();
	if ((PrevState != NULL) && (NextState != NULL))
	{
		const TSharedRef<SNode>* pPrevNodeWidget = NodeToWidgetLookup.Find(PrevState);
		const TSharedRef<SNode>* pNextNodeWidget = NodeToWidgetLookup.Find(NextState);
		if ((pPrevNodeWidget != NULL) && (pNextNodeWidget != NULL))
		{
			const TSharedRef<SNode>& PrevNodeWidget = *pPrevNodeWidget;
			const TSharedRef<SNode>& NextNodeWidget = *pNextNodeWidget;

			StartGeom = FGeometry(FVector2D(PrevState->NodePosX, PrevState->NodePosY), FVector2D::ZeroVector, PrevNodeWidget->GetDesiredSize(), 1.0f);
			EndGeom = FGeometry(FVector2D(NextState->NodePosX, NextState->NodePosY), FVector2D::ZeroVector, NextNodeWidget->GetDesiredSize(), 1.0f);

			TArray<UDungeonRulesNode_Transition*> Transitions;
			PrevState->GetTransitionList(Transitions);

			Transitions = Transitions.FilterByPredicate([NextState](const UDungeonRulesNode_Transition* InTransition) -> bool
			{
				return InTransition->GetNextState() == NextState;
			});

			TransIndex = Transitions.IndexOfByKey(TransNode);
			NumOfTrans = Transitions.Num();

			PrevStateNodeWidgetPtr = PrevNodeWidget;
		}
	}

	//Position Node
	PositionBetweenTwoNodesWithOffset(StartGeom, EndGeom, TransIndex, NumOfTrans);
}

TSharedRef<SWidget> SGraphNodeDungeonRules_Transition::GenerateRichTooltip()
{
	UDungeonRulesNode_Transition* TransNode = CastChecked<UDungeonRulesNode_Transition>(GraphNode);

	TSharedRef<SVerticalBox> Widget = SNew(SVerticalBox);

	const FText TooltipName = GetPreviewCornerText(false);
	const FText TooltipDesc = GraphNode->GetTooltipText();

	Widget->AddSlot()
		.AutoHeight()
		.Padding( 2.0f )
		[
			SNew(STextBlock)
			.TextStyle( FAppStyle::Get(), TEXT("Graph.TransitionNode.TooltipName") )
			.Text(TooltipName)
		];

	Widget->AddSlot()
		.AutoHeight()
		.Padding(2.0f)
		[
			SNew(STextBlock)
				.TextStyle(FAppStyle::Get(), TEXT("Graph.TransitionNode.TooltipRule"))
				.Text(TooltipDesc)
		];

	Widget->AddSlot()
		.AutoHeight()
		.Padding( 2.0f )
		[
			IDocumentation::Get()->CreateToolTip(FText::FromString("Documentation"), nullptr, TransNode->GetDocumentationLink(), TransNode->GetDocumentationExcerptName())
		];

	return Widget;
}

TSharedPtr<SToolTip> SGraphNodeDungeonRules_Transition::GetComplexTooltip()
{
	return SNew(SToolTip)
		[
			GenerateRichTooltip()
		];
}

void SGraphNodeDungeonRules_Transition::UpdateGraphNode()
{
	InputPins.Empty();
	OutputPins.Empty();

	// Reset variables that are going to be exposed, in case we are refreshing an already setup node.
	RightNodeBox.Reset();
	LeftNodeBox.Reset();

	this->ContentScale.Bind( this, &SGraphNode::GetContentScale );
	this->GetOrAddSlot( ENodeZone::Center )
		.HAlign(HAlign_Center)
		.VAlign(VAlign_Center)
		[
			SNew(SOverlay)
			+SOverlay::Slot()
			[
				SNew(SImage)
				.Image( FAppStyle::GetBrush("Graph.TransitionNode.ColorSpill") )
				.ColorAndOpacity( this, &SGraphNodeDungeonRules_Transition::GetTransitionColor )
			]
#if false // TODO: should be kept or removed?
			+SOverlay::Slot()
			[
				// TODO: remove icon and write the priority number instead
				SNew(SImage)
				.Image( this, &SGraphNodeDungeonRules_Transition::GetTransitionIconImage )
			]
#endif
			+ SOverlay::Slot()
			.HAlign(HAlign_Center)
			.VAlign(VAlign_Center)
			.Padding(5.0f)
			[
				SNew(STextBlock)
				.Text(this, &SGraphNodeDungeonRules_Transition::GetTransitionPriorityOrder)
				.ColorAndOpacity(FLinearColor::Black)
				.Justification(ETextJustify::Center)
				.MinDesiredWidth(15.0f)
			]
		];
}

FText SGraphNodeDungeonRules_Transition::GetPreviewCornerText(bool bReverse) const
{
	UDungeonRulesNode_Transition* TransNode = CastChecked<UDungeonRulesNode_Transition>(GraphNode);

	UDungeonRulesNode* PrevState = (bReverse ? TransNode->GetNextState() : TransNode->GetPreviousState());
	UDungeonRulesNode* NextState = (bReverse ? TransNode->GetPreviousState() : TransNode->GetNextState());

	FText Result = LOCTEXT("BadTransition", "Bad transition (missing source or target)");

	// Show the priority if there is any ambiguity
	if (PrevState != NULL)
	{
		if (NextState != NULL)
		{
			const UDungeonRuleTransition* TransitionInstance = TransNode->GetNodeInstance<UDungeonRuleTransition>();
			int32 ThisPriorityOrder = TransitionInstance->PriorityOrder;

			TArray<UDungeonRulesNode_Transition*> TransitionFromSource;
			PrevState->GetTransitionList(/*out*/ TransitionFromSource);

			bool bMultiplePriorities = false;
			if (TransitionFromSource.Num() > 1)
			{
				// See if the priorities differ
				for (int32 Index = 0; (Index < TransitionFromSource.Num()) && !bMultiplePriorities; ++Index)
				{
					const UDungeonRuleTransition* OtherTransition = TransitionFromSource[Index]->GetNodeInstance<UDungeonRuleTransition>();
					if (!OtherTransition)
						continue;

					const bool bDifferentPriority = (OtherTransition->PriorityOrder != ThisPriorityOrder);
					bMultiplePriorities |= bDifferentPriority;
				}
			}

			if (bMultiplePriorities)
			{
				Result = FText::Format(LOCTEXT("TransitionXToYWithPriority", "{0} to {1} (Priority {2})"), FText::FromString(PrevState->GetStateName()), FText::FromString(NextState->GetStateName()), FText::AsNumber(ThisPriorityOrder));
			}
			else
			{
				Result = FText::Format(LOCTEXT("TransitionXToY", "{0} to {1}"), FText::FromString(PrevState->GetStateName()), FText::FromString(NextState->GetStateName()));
			}
		}
	}

	return Result;
}

FLinearColor SGraphNodeDungeonRules_Transition::StaticGetTransitionColor(UDungeonRulesNode_Transition* TransNode, bool bIsHovered)
{
	//@TODO: Make configurable by styling
	const FLinearColor HoverColor(0.724f, 0.256f, 0.0f, 1.0f);
	FLinearColor BaseColor(0.9f, 0.9f, 0.9f, 1.0f);
	return bIsHovered ? HoverColor : BaseColor;
}

FSlateColor SGraphNodeDungeonRules_Transition::GetTransitionColor() const
{	
	// Highlight the transition node when the node is hovered or when the previous state is hovered
	UDungeonRulesNode_Transition* TransNode = CastChecked<UDungeonRulesNode_Transition>(GraphNode);
	return StaticGetTransitionColor(TransNode, (IsHovered() || (PrevStateNodeWidgetPtr.IsValid() && PrevStateNodeWidgetPtr.Pin()->IsHovered())));
}

const FSlateBrush* SGraphNodeDungeonRules_Transition::GetTransitionIconImage() const
{
	UDungeonRulesNode_Transition* TransNode = CastChecked<UDungeonRulesNode_Transition>(GraphNode);
	return FAppStyle::GetBrush("Graph.TransitionNode.Icon");
}

FText SGraphNodeDungeonRules_Transition::GetTransitionPriorityOrder() const
{
	UDungeonRulesNode_Transition* TransNode = CastChecked<UDungeonRulesNode_Transition>(GraphNode);
	const UDungeonRuleTransition* TransInstance = TransNode->GetNodeInstance<UDungeonRuleTransition>();
	return TransInstance ? FText::AsNumber(TransInstance->PriorityOrder) : FText::FromString("?");
}

void SGraphNodeDungeonRules_Transition::OnMouseEnter(const FGeometry& MyGeometry, const FPointerEvent& MouseEvent)
{
	UDungeonRulesNode_Transition* TransNode = CastChecked<UDungeonRulesNode_Transition>(GraphNode);
	if (UEdGraphPin* Pin = TransNode->GetInputPin())
	{
		GetOwnerPanel()->AddPinToHoverSet(Pin);
	}

	SGraphNode::OnMouseEnter(MyGeometry, MouseEvent);
}

void SGraphNodeDungeonRules_Transition::OnMouseLeave(const FPointerEvent& MouseEvent)
{	
	UDungeonRulesNode_Transition* TransNode = CastChecked<UDungeonRulesNode_Transition>(GraphNode);
	if (UEdGraphPin* Pin = TransNode->GetInputPin())
	{
		GetOwnerPanel()->RemovePinFromHoverSet(Pin);
	}

	SGraphNode::OnMouseLeave(MouseEvent);
}

void SGraphNodeDungeonRules_Transition::PositionBetweenTwoNodesWithOffset(const FGeometry& StartGeom, const FGeometry& EndGeom, int32 NodeIndex, int32 MaxNodes) const
{
	// Get a reasonable seed point (halfway between the boxes)
	const FVector2D StartCenter = FGeometryHelper::CenterOf(StartGeom);
	const FVector2D EndCenter = FGeometryHelper::CenterOf(EndGeom);
	const FVector2D SeedPoint = (StartCenter + EndCenter) * 0.5f;

	// Find the (approximate) closest points between the two boxes
	const FVector2D StartAnchorPoint = FGeometryHelper::FindClosestPointOnGeom(StartGeom, SeedPoint);
	const FVector2D EndAnchorPoint = FGeometryHelper::FindClosestPointOnGeom(EndGeom, SeedPoint);

	// Position ourselves halfway along the connecting line between the nodes, elevated away perpendicular to the direction of the line
	const float Height = 30.0f;

	const FVector2D DesiredNodeSize = GetDesiredSize();

	FVector2D DeltaPos(EndAnchorPoint - StartAnchorPoint);

	if (DeltaPos.IsNearlyZero())
	{
		DeltaPos = FVector2D(10.0f, 0.0f);
	}

	const FVector2D Normal = FVector2D(DeltaPos.Y, -DeltaPos.X).GetSafeNormal();

	const FVector2D NewCenter = StartAnchorPoint + (0.5f * DeltaPos) + (Height * Normal);

	FVector2D DeltaNormal = DeltaPos.GetSafeNormal();
	
	// Calculate node offset in the case of multiple transitions between the same two nodes
	// MultiNodeOffset: the offset where 0 is the centre of the transition, -1 is 1 <size of node>
	// towards the PrevStateNode and +1 is 1 <size of node> towards the NextStateNode.

	const float MutliNodeSpace = 0.2f; // Space between multiple transition nodes (in units of <size of node> )
	const float MultiNodeStep = (1.f + MutliNodeSpace); //Step between node centres (Size of node + size of node spacer)

	const float MultiNodeStart = -((MaxNodes - 1) * MultiNodeStep) / 2.f;
	const float MultiNodeOffset = MultiNodeStart + (NodeIndex * MultiNodeStep);

	// Now we need to adjust the new center by the node size, zoom factor and multi node offset
	const FVector2D NewCorner = NewCenter - (0.5f * DesiredNodeSize) + (DeltaNormal * MultiNodeOffset * DesiredNodeSize.Size());

	GraphNode->NodePosX = static_cast<int32>(NewCorner.X);
	GraphNode->NodePosY = static_cast<int32>(NewCorner.Y);
}

/////////////////////////////////////////////////////

#undef LOCTEXT_NAMESPACE
