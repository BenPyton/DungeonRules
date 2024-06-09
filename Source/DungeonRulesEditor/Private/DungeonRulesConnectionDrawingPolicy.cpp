// Copyright Epic Games, Inc. All Rights Reserved.

#include "DungeonRulesConnectionDrawingPolicy.h"

#include "Nodes/RuleEntryNode.h"
#include "Nodes/RuleExitNode.h"
#include "Nodes/RuleNodeBase.h"
#include "Nodes/RuleTransitionNode.h"
#include "NodeSlates/SGraphNodeDungeonRuleTransition.h"
#include "Containers/Set.h"
#include "EdGraph/EdGraphPin.h"
#include "Layout/ArrangedChildren.h"
#include "Layout/ArrangedWidget.h"
#include "Layout/PaintGeometry.h"
#include "Math/Color.h"
#include "Math/UnrealMathSSE.h"
#include "Misc/Optional.h"
#include "Rendering/DrawElements.h"
#include "Rendering/RenderingCommon.h"
#include "SGraphNode.h"
#include "SGraphPin.h"
#include "Styling/SlateBrush.h"
#include "Templates/Casts.h"
#include "Math/UnrealMathUtility.h"
#include "Brushes/SlateRoundedBoxBrush.h"
#include "Styling/StyleColors.h"
#include "Nodes/RuleConduitNode.h"

class FSlateRect;
class SWidget;
struct FGeometry;

/////////////////////////////////////////////////////
// FDungeonRulesConnectionDrawingPolicy

FDungeonRulesConnectionDrawingPolicy::FDungeonRulesConnectionDrawingPolicy(int32 InBackLayerID, int32 InFrontLayerID, float ZoomFactor, const FSlateRect& InClippingRect, FSlateWindowElementList& InDrawElements, UEdGraph* InGraphObj)
	: FConnectionDrawingPolicy(InBackLayerID, InFrontLayerID, ZoomFactor, InClippingRect, InDrawElements)
	, GraphObj(InGraphObj)
{
}

void FDungeonRulesConnectionDrawingPolicy::DetermineWiringStyle(UEdGraphPin* OutputPin, UEdGraphPin* InputPin, /*inout*/ FConnectionParams& Params)
{
	Params.bUserFlag2 = true;
	Params.AssociatedPin1 = OutputPin;
	Params.AssociatedPin2 = InputPin;
	Params.WireThickness = 1.5f;

	if (InputPin)
	{
		if (URuleTransitionNode* TransNode = Cast<URuleTransitionNode>(InputPin->GetOwningNode()))
		{
			const bool IsInputPinHovered = HoveredPins.Contains(InputPin);
			Params.WireColor = SGraphNodeDungeonRuleTransition::StaticGetTransitionColor(TransNode, IsInputPinHovered);
		}
	}

	const bool bDeemphasizeUnhoveredPins = HoveredPins.Num() > 0;
	if (bDeemphasizeUnhoveredPins)
	{
		// These "Valid" and "MaybeInvalid" pins are a hack in case the preview arrow is dragged from the input pin (e.g. from the exit node)
		// Because "ApplyHoverDeemphasis" function will need a valid "Valid" pin, and "MabeInvalid" pin can be nullptr in order to make it emphasized.
		const bool bPreviewLinkFromInputPin = (!OutputPin && InputPin);
		UEdGraphPin* ValidPin = (bPreviewLinkFromInputPin) ? InputPin : OutputPin;
		UEdGraphPin* MaybeInvalidPin = (bPreviewLinkFromInputPin) ? OutputPin : InputPin;
		ApplyHoverDeemphasis(ValidPin, MaybeInvalidPin, /*inout*/ Params.WireThickness, /*inout*/ Params.WireColor);
	}

	// Make the transition that is currently relinked, semi-transparent.
	for (const FRelinkConnection& Connection : RelinkConnections)
	{
		if (InputPin == nullptr || OutputPin == nullptr)
		{
			continue;
		}
		const FGraphPinHandle SourcePinHandle = Connection.SourcePin;
		const FGraphPinHandle TargetPinHandle = Connection.TargetPin;

		// Skip all transitions that don't start at the node our dragged and relink transition starts from
		if (OutputPin->GetOwningNode()->NodeGuid == SourcePinHandle.NodeGuid)
		{
			// Safety check to verify if the node is a transition node
			if (URuleTransitionNode* TransitonNode = Cast<URuleTransitionNode>(InputPin->GetOwningNode()))
			{
				if (UEdGraphPin* TransitionOutputPin = TransitonNode->GetOutputPin())
				{
					if (TargetPinHandle.NodeGuid == TransitionOutputPin->GetOwningNode()->NodeGuid)
					{
						Params.WireColor.A *= 0.2f;
					}
				}
			}
		}
	}
}

void FDungeonRulesConnectionDrawingPolicy::DetermineLinkGeometry(
	FArrangedChildren& ArrangedNodes, 
	TSharedRef<SWidget>& OutputPinWidget,
	UEdGraphPin* OutputPin,
	UEdGraphPin* InputPin,
	/*out*/ FArrangedWidget*& StartWidgetGeometry,
	/*out*/ FArrangedWidget*& EndWidgetGeometry
	)
{
	if (URuleEntryNode* EntryNode = Cast<URuleEntryNode>(OutputPin->GetOwningNode()))
	{
		StartWidgetGeometry = PinGeometries->Find(OutputPinWidget);

		URuleNodeBase* State = CastChecked<URuleNodeBase>(InputPin->GetOwningNode());
		int32 StateIndex = NodeWidgetMap.FindChecked(State);
		EndWidgetGeometry = &(ArrangedNodes[StateIndex]);
	}
	else if (URuleTransitionNode* TransNode = Cast<URuleTransitionNode>(InputPin->GetOwningNode()))
	{
		URuleNodeBase* PrevState = TransNode->GetPreviousState();
		URuleNodeBase* NextState = TransNode->GetNextState();
		if ((PrevState != NULL) && (NextState != NULL))
		{
			int32* PrevNodeIndex = NodeWidgetMap.Find(PrevState);
			int32* NextNodeIndex = NodeWidgetMap.Find(NextState);
			if ((PrevNodeIndex != NULL) && (NextNodeIndex != NULL))
			{
				StartWidgetGeometry = &(ArrangedNodes[*PrevNodeIndex]);
				EndWidgetGeometry = &(ArrangedNodes[*NextNodeIndex]);
			}
		}
	}
	else
	{
		StartWidgetGeometry = PinGeometries->Find(OutputPinWidget);

		if (TSharedPtr<SGraphPin>* pTargetWidget = PinToPinWidgetMap.Find(InputPin))
		{
			TSharedRef<SGraphPin> InputWidget = (*pTargetWidget).ToSharedRef();
			EndWidgetGeometry = PinGeometries->Find(InputWidget);
		}
	}
}

void FDungeonRulesConnectionDrawingPolicy::Draw(TMap<TSharedRef<SWidget>, FArrangedWidget>& InPinGeometries, FArrangedChildren& ArrangedNodes)
{
	// Build an acceleration structure to quickly find geometry for the nodes
	NodeWidgetMap.Empty();
	for (int32 NodeIndex = 0; NodeIndex < ArrangedNodes.Num(); ++NodeIndex)
	{
		FArrangedWidget& CurWidget = ArrangedNodes[NodeIndex];
		TSharedRef<SGraphNode> ChildNode = StaticCastSharedRef<SGraphNode>(CurWidget.Widget);
		NodeWidgetMap.Add(ChildNode->GetNodeObj(), NodeIndex);
	}

	// Now draw
	FConnectionDrawingPolicy::Draw(InPinGeometries, ArrangedNodes);
}

void FDungeonRulesConnectionDrawingPolicy::DrawPreviewConnector(const FGeometry& PinGeometry, const FVector2D& StartPoint, const FVector2D& EndPoint, UEdGraphPin* Pin)
{
	FConnectionParams Params;
	UEdGraphPin* InputPin = (Pin->Direction == EGPD_Input) ? Pin : nullptr;
	UEdGraphPin* OutputPin = (Pin->Direction == EGPD_Output) ? Pin : nullptr;
	DetermineWiringStyle(OutputPin, InputPin, /*inout*/ Params);

	const FVector2D OutputPoint = (OutputPin) ? Internal_FindLineAnchorPoint(EndPoint, PinGeometry, OutputPin) : StartPoint;
	const FVector2D InputPoint = (InputPin) ? Internal_FindLineAnchorPoint(StartPoint, PinGeometry, InputPin) : EndPoint;

	Params.bUserFlag2 = false; // bUserFlag2 is used to indicate whether the drawn arrow is a preview transition (the temporary transition when creating or relinking).
	DrawSplineWithArrow(OutputPoint, InputPoint, Params);
}


void FDungeonRulesConnectionDrawingPolicy::DrawSplineWithArrow(const FVector2D& StartAnchorPoint, const FVector2D& EndAnchorPoint, const FConnectionParams& Params)
{
	Internal_DrawLineWithArrow(StartAnchorPoint, EndAnchorPoint, Params);
}

void FDungeonRulesConnectionDrawingPolicy::Internal_DrawLineWithArrow(const FVector2D& StartAnchorPoint, const FVector2D& EndAnchorPoint, const FConnectionParams& Params)
{
	//@TODO: Should this be scaled by zoom factor?
	const float LineSeparationAmount = 4.5f;

	const FVector2D DeltaPos = EndAnchorPoint - StartAnchorPoint;
	const FVector2D UnitDelta = DeltaPos.GetSafeNormal();
	const FVector2D Normal = FVector2D(DeltaPos.Y, -DeltaPos.X).GetSafeNormal();

	// Come up with the final start/end points
	const FVector2D DirectionBias = Normal * LineSeparationAmount;
	const FVector2D LengthBias = ArrowRadius.X * UnitDelta;
	const FVector2D StartPoint = StartAnchorPoint + DirectionBias + LengthBias;
	const FVector2D EndPoint = EndAnchorPoint + DirectionBias - LengthBias;
	FLinearColor ArrowHeadColor = Params.WireColor;

	// Draw a line/spline
	DrawConnection(WireLayerID, StartPoint, EndPoint, Params);

	const FVector2D ArrowDrawPos = EndPoint - ArrowRadius;
	const double AngleInRadians = FMath::Atan2(DeltaPos.Y, DeltaPos.X);

	// Draw the transition grab handles in case the mouse is hovering the transition
	bool StartHovered = false;
	bool EndHovered = false;
	const FVector FVecMousePos = FVector(LocalMousePosition.X, LocalMousePosition.Y, 0.0f);
	const FVector ClosestPoint = FMath::ClosestPointOnSegment(FVecMousePos,
		FVector(StartPoint.X, StartPoint.Y, 0.0f),
		FVector(EndPoint.X, EndPoint.Y, 0.0f));
	if ((ClosestPoint - FVecMousePos).Length() < RelinkHandleHoverRadius)
	{
		StartHovered = FVector2D(StartPoint - LocalMousePosition).Length() < RelinkHandleHoverRadius;
		EndHovered = FVector2D(EndPoint - LocalMousePosition).Length() < RelinkHandleHoverRadius;
		FVector2D HoverIndicatorPosition = StartHovered ? StartPoint : EndPoint;

		// Set the hovered pin results. This will be used by the SGraphPanel again.
		const float SquaredDistToPin1 = (Params.AssociatedPin1 != nullptr) ? (StartPoint - LocalMousePosition).SizeSquared() : FLT_MAX;
		const float SquaredDistToPin2 = (Params.AssociatedPin2 != nullptr) ? (EndPoint - LocalMousePosition).SizeSquared() : FLT_MAX;
		if (EndHovered)
		{
			SplineOverlapResult = FGraphSplineOverlapResult(Params.AssociatedPin1, Params.AssociatedPin2, SquaredDistToPin2, SquaredDistToPin1, SquaredDistToPin2, true);
		}

		// Draw grab handles only in case no relinking operation is performed
		if (RelinkConnections.IsEmpty() && Params.bUserFlag2)
		{
			if (EndHovered)
			{
				// Draw solid orange circle behind the arrow head in case the arrow head is hovered (area that enables a relink).
				FSlateRoundedBoxBrush RoundedBoxBrush = FSlateRoundedBoxBrush(
					FLinearColor(0.0f, 0.0f, 0.0f, 0.0f), 9.0f, FStyleColors::AccentOrange, 100.0f);

				FSlateDrawElement::MakeBox(DrawElementsList,
					ArrowLayerID-1, // Draw behind the arrow
					FPaintGeometry(EndPoint - ArrowRadius, BubbleImage->ImageSize * ZoomFactor, ZoomFactor),
					&RoundedBoxBrush);

				ArrowHeadColor = FLinearColor::Black;
			}
			else
			{
				// Draw circle around the arrow in case the transition is hovered (mouse close or over transition line or arrow head).
				const int CircleLineSegments = 16;
				const float CircleRadius = 10.0f;
				const FVector2D CircleCenter = EndPoint - UnitDelta * 2.0f;
				DrawCircle(CircleCenter, CircleRadius, Params.WireColor, CircleLineSegments);
			}
		}
	}

	// Draw the number of relinked transitions on the preview transition.
	if (!RelinkConnections.IsEmpty() && !Params.bUserFlag2)
	{
		// Get the number of actually relinked transitions.
		int32 NumRelinkedTransitions = 0;
		for (const FRelinkConnection& Connection : RelinkConnections)
		{
			NumRelinkedTransitions += URuleTransitionNode::GetListTransitionNodesToRelink(Connection.SourcePin, Connection.TargetPin, SelectedGraphNodes).Num();

			if (URuleEntryNode* EntryNode = Cast<URuleEntryNode>(Connection.SourcePin->GetOwningNode()))
			{
				NumRelinkedTransitions += 1;
			}
		}

		const FVector2D TransitionCenter = StartAnchorPoint + DeltaPos * 0.5f;
		const FVector2D TextPosition = TransitionCenter + Normal * 15.0f * ZoomFactor;

		FSlateDrawElement::MakeText(
			DrawElementsList,
			ArrowLayerID,
			FPaintGeometry(TextPosition, ArrowImage->ImageSize * ZoomFactor, ZoomFactor),
			FText::AsNumber(NumRelinkedTransitions),
			FCoreStyle::Get().GetFontStyle("SmallFont"));
	}

	// Draw the transition arrow triangle
	FSlateDrawElement::MakeRotatedBox(
		DrawElementsList,
		ArrowLayerID,
		FPaintGeometry(ArrowDrawPos, ArrowImage->ImageSize * ZoomFactor, ZoomFactor),
		ArrowImage,
		ESlateDrawEffect::None,
		static_cast<float>(AngleInRadians),
		TOptional<FVector2D>(),
		FSlateDrawElement::RelativeToElement,
		ArrowHeadColor
	);
}

struct FDungeonGeometryHelper
{
	static void ConvertToRotatedPoints(const FGeometry& Geom, TArray<FVector2D>& Points, float AngleInRadian)
	{
		const FVector2D Size = Geom.GetDrawSize();
		const FVector2D Location = FVector2D(Geom.AbsolutePosition);
		const FVector2D Center = Location + 0.5 * Size;
		const FVector2D LocalLocation = Location - Center;
		const FVector2D Rotation = FVector2D(cos(AngleInRadian), sin(AngleInRadian));
		const FVector2D LocalPoints[4] = {
			FVector2D(0.0f, 0.0f),
			FVector2D(0.0f, Size.Y),
			FVector2D(Size.X, Size.Y),
			FVector2D(Size.X, 0.0f)
		};

		int32 Index = Points.AddUninitialized(4);
		for (int32 i = 0; i < 4; ++i)
		{
			FVector2D CurrentLocation = LocalLocation + LocalPoints[i];
			Points[Index++] = Center + FVector2D(
				CurrentLocation.X * Rotation.X - CurrentLocation.Y * Rotation.Y,
				CurrentLocation.X * Rotation.Y + CurrentLocation.Y * Rotation.X);
		}
	}

	static FVector2D FindClosestPointOnRotatedGeom(const FGeometry& Geom, const FVector2D& TestPoint, float AngleInRadian)
	{
		TArray<FVector2D> Points;
		ConvertToRotatedPoints(Geom, Points, AngleInRadian);

		float BestDistanceSquared = MAX_FLT;
		FVector2D BestPoint;
		for (int32 i = 0; i < Points.Num(); ++i)
		{
			const FVector2D Candidate = FGeometryHelper::FindClosestPointOnLine(Points[i], Points[(i + 1) % Points.Num()], TestPoint);
			const float CandidateDistanceSquared = (Candidate - TestPoint).SizeSquared();
			if (CandidateDistanceSquared < BestDistanceSquared)
			{
				BestPoint = Candidate;
				BestDistanceSquared = CandidateDistanceSquared;
			}
		}

		return BestPoint;
	}
};

FVector2D FDungeonRulesConnectionDrawingPolicy::Internal_FindLineAnchorPoint(const FVector2D& SeedPoint, const FGeometry& Geom, const UEdGraphPin* Pin) const
{
	if (Pin)
	{
		const URuleConduitNode* Conduit = Cast<URuleConduitNode>(Pin->GetOwningNode());
		if (!Conduit)
		{
			if (const URuleTransitionNode* Transition = Cast<URuleTransitionNode>(Pin->GetOwningNode()))
			{
				Conduit = Cast<URuleConduitNode>((Pin->Direction == EGPD_Input) ? Transition->GetNextState() : Transition->GetPreviousState());
			}
		}

		if (Conduit)
		{
			return FDungeonGeometryHelper::FindClosestPointOnRotatedGeom(Geom, SeedPoint, FMath::DegreesToRadians(-45.0f));
		}
	}

	return FGeometryHelper::FindClosestPointOnGeom(Geom, SeedPoint);
}

void FDungeonRulesConnectionDrawingPolicy::DrawCircle(const FVector2D& Center, float Radius, const FLinearColor& Color, const int NumLineSegments)
{
	TempPoints.Empty();

	const float NumFloatLineSegments = (float)NumLineSegments;
	for (int i = 0; i <= NumLineSegments; i++)
	{
		const float Angle = (i / NumFloatLineSegments) * TWO_PI;

		FVector2D PointOnCircle;
		PointOnCircle.X = cosf(Angle) * Radius;
		PointOnCircle.Y = sinf(Angle) * Radius;
		TempPoints.Add(PointOnCircle);
	}

	FSlateDrawElement::MakeLines(
		DrawElementsList,
		ArrowLayerID + 1,
		FPaintGeometry(Center, FVector2D(Radius, Radius) * ZoomFactor, ZoomFactor),
		TempPoints,
		ESlateDrawEffect::None,
		Color
	);
}

void FDungeonRulesConnectionDrawingPolicy::DrawSplineWithArrow(const FGeometry& StartGeom, const FGeometry& EndGeom, const FConnectionParams& Params)
{
	// Get a reasonable seed point (halfway between the boxes)
	const FVector2D StartCenter = FGeometryHelper::CenterOf(StartGeom);
	const FVector2D EndCenter = FGeometryHelper::CenterOf(EndGeom);
	const FVector2D SeedPoint = (StartCenter + EndCenter) * 0.5f;

	// Find the (approximate) closest points between the two boxes
	const FVector2D StartAnchorPoint = Internal_FindLineAnchorPoint(SeedPoint, StartGeom, Params.AssociatedPin1);
	const FVector2D EndAnchorPoint = Internal_FindLineAnchorPoint(SeedPoint, EndGeom, Params.AssociatedPin2);

	DrawSplineWithArrow(StartAnchorPoint, EndAnchorPoint, Params);
}

FVector2D FDungeonRulesConnectionDrawingPolicy::ComputeSplineTangent(const FVector2D& Start, const FVector2D& End) const
{
	const FVector2D Delta = End - Start;
	const FVector2D NormDelta = Delta.GetSafeNormal();

	return NormDelta;
}
