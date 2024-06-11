// Copyright Epic Games, Inc. All Rights Reserved.

#include "SGraphNodeDungeonRuleConduit.h"

#include "Nodes/RuleConduitNode.h"
#include "Nodes/RuleNodeBase.h"
#include "Animation/AnimBlueprint.h"
#include "Animation/AnimBlueprintGeneratedClass.h"
#include "Animation/AnimInstance.h"
#include "Containers/Map.h"
#include "Delegates/Delegate.h"
#include "EdGraph/EdGraphNode.h"
#include "EdGraph/EdGraphPin.h"
#include "Engine/Blueprint.h"
#include "GenericPlatform/ICursor.h"
#include "HAL/PlatformCrt.h"
#include "IDocumentation.h"
#include "Internationalization/Internationalization.h"
#include "Kismet2/BlueprintEditorUtils.h"
#include "Layout/Margin.h"
#include "Layout/Visibility.h"
#include "Math/UnrealMathSSE.h"
#include "Math/Vector2D.h"
#include "Misc/AssertionMacros.h"
#include "Misc/Attribute.h"
#include "Misc/Optional.h"
#include "SGraphPanel.h"
#include "SGraphPin.h"
#include "SGraphPreviewer.h"
#include "SNodePanel.h"
#include "SlotBase.h"
#include "Styling/AppStyle.h"
#include "Templates/Casts.h"
#include "Types/SlateEnums.h"
#include "Widgets/Images/SImage.h"
#include "Widgets/Layout/SBorder.h"
#include "Widgets/Notifications/SErrorText.h"
#include "Widgets/SBoxPanel.h"
#include "Widgets/SOverlay.h"
#include "Widgets/SToolTip.h"
#include "Widgets/Text/SInlineEditableTextBlock.h"
#include "Widgets/Text/STextBlock.h"
#include "Styling/SlateStyleRegistry.h"

class SWidget;
class UEdGraphSchema;
struct FGeometry;
struct FPointerEvent;
struct FSlateBrush;

#define LOCTEXT_NAMESPACE "SGraphNodeDungeonRule"

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
// SGraphNodeDungeonRule

void SGraphNodeDungeonRuleConduit::Construct(const FArguments& InArgs, URuleNodeBase* InNode)
{
	this->GraphNode = InNode;

	this->SetCursor(EMouseCursor::CardinalCross);

	this->UpdateGraphNode();
}

#if false // Blueprint
void SGraphNodeDungeonRuleConduit::GetStateInfoPopup(UEdGraphNode* GraphNode, TArray<FGraphInformationPopupInfo>& Popups)
{
	UAnimBlueprint* AnimBlueprint = Cast<UAnimBlueprint>(FBlueprintEditorUtils::FindBlueprintForNode(GraphNode));
	if(AnimBlueprint)
	{
		UAnimInstance* ActiveObject = Cast<UAnimInstance>(AnimBlueprint->GetObjectBeingDebugged());
		UAnimBlueprintGeneratedClass* Class = AnimBlueprint->GetAnimBlueprintGeneratedClass();

		FLinearColor CurrentStateColor(1.f, 0.5f, 0.25f);

		// Display various types of debug data
		if ((ActiveObject != NULL) && (Class != NULL))
		{
			if (Class->GetAnimNodeProperties().Num())
			{
				if (FStateMachineDebugData* DebugInfo = Class->GetAnimBlueprintDebugData().StateMachineDebugData.Find(GraphNode->GetGraph()))
				{
					if(int32* StateIndexPtr = DebugInfo->NodeToStateIndex.Find(GraphNode))
					{
						for(const FStateMachineStateDebugData& StateData : Class->GetAnimBlueprintDebugData().StateData)
						{
							if(StateData.StateMachineIndex == DebugInfo->MachineIndex && StateData.StateIndex == *StateIndexPtr)
							{
								if (StateData.Weight > 0.0f)
								{
									FText StateText;
									if (StateData.ElapsedTime > 0.0f)
									{
										StateText = FText::Format(LOCTEXT("ActiveStateWeightFormat", "{0}\nActive for {1}s"), FText::AsPercent(StateData.Weight), FText::AsNumber(StateData.ElapsedTime));
									}
									else
									{
										StateText = FText::Format(LOCTEXT("StateWeightFormat", "{0}"), FText::AsPercent(StateData.Weight));
									}

									Popups.Emplace(nullptr, CurrentStateColor, StateText.ToString());
									break;
								}
							}
						}
					}
				}
			}
		}
	}
}

void SGraphNodeDungeonRuleConduit::GetNodeInfoPopups(FNodeInfoContext* Context, TArray<FGraphInformationPopupInfo>& Popups) const
{
	GetStateInfoPopup(GraphNode, Popups);
}
#endif

FSlateColor SGraphNodeDungeonRuleConduit::GetBorderBackgroundColor() const
{
	FLinearColor InactiveStateColor(0.08f, 0.08f, 0.08f);
	FLinearColor ActiveStateColorDim(0.4f, 0.3f, 0.15f);
	FLinearColor ActiveStateColorBright(1.f, 0.6f, 0.35f);

	return GetBorderBackgroundColor_Internal(InactiveStateColor, ActiveStateColorDim, ActiveStateColorBright);
}

FSlateColor SGraphNodeDungeonRuleConduit::GetBorderBackgroundColor_Internal(FLinearColor InactiveStateColor, FLinearColor ActiveStateColorDim, FLinearColor ActiveStateColorBright) const
{
#if false // Blueprint
	UAnimBlueprint* AnimBlueprint = Cast<UAnimBlueprint>(FBlueprintEditorUtils::FindBlueprintForNode(GraphNode));
	if(AnimBlueprint)
	{
		UAnimInstance* ActiveObject = Cast<UAnimInstance>(AnimBlueprint->GetObjectBeingDebugged());
		UAnimBlueprintGeneratedClass* Class = AnimBlueprint->GetAnimBlueprintGeneratedClass();

		// Display various types of debug data
		if ((ActiveObject != NULL) && (Class != NULL))
		{
			if (FStateMachineDebugData* DebugInfo = Class->GetAnimBlueprintDebugData().StateMachineDebugData.Find(GraphNode->GetGraph()))
			{
				if(int32* StateIndexPtr = DebugInfo->NodeToStateIndex.Find(GraphNode))
				{
					for(const FStateMachineStateDebugData& StateData : Class->GetAnimBlueprintDebugData().StateData)
					{
						if(StateData.StateMachineIndex == DebugInfo->MachineIndex && StateData.StateIndex == *StateIndexPtr)
						{
							if (StateData.Weight > 0.0f)
							{
								return FMath::Lerp<FLinearColor>(ActiveStateColorDim, ActiveStateColorBright, StateData.Weight);
							}
						}
					}
				}
			}
		}
	}
#endif

	return InactiveStateColor;
}

void SGraphNodeDungeonRuleConduit::OnMouseEnter(const FGeometry& MyGeometry, const FPointerEvent& MouseEvent)
{
	// Add pins to the hover set so outgoing transitions arrows remains highlighted while the mouse is over the state node
	if (const URuleNodeBase* StateNode = Cast<URuleNodeBase>(GraphNode))
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

void SGraphNodeDungeonRuleConduit::OnMouseLeave(const FPointerEvent& MouseEvent)
{
	// Remove manually added pins from the hover set
	if (const URuleNodeBase* StateNode = Cast<URuleNodeBase>(GraphNode))
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

const FSlateBrush* SGraphNodeDungeonRuleConduit::GetShadowBrush(bool bSelected) const
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

void SGraphNodeDungeonRuleConduit::UpdateGraphNode()
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
			.BorderBackgroundColor(this, &SGraphNodeDungeonRuleConduit::GetBorderBackgroundColor)
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

void SGraphNodeDungeonRuleConduit::CreatePinWidgets()
{
	URuleNodeBase* StateNode = CastChecked<URuleNodeBase>(GraphNode);

	UEdGraphPin* CurPin = StateNode->GetOutputPin();
	if (!CurPin->bHidden)
	{
		TSharedPtr<SGraphPin> NewPin = SNew(SConduitOutputPin, CurPin);

		this->AddPin(NewPin.ToSharedRef());
	}
}

void SGraphNodeDungeonRuleConduit::AddPin(const TSharedRef<SGraphPin>& PinToAdd)
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

TSharedPtr<SToolTip> SGraphNodeDungeonRuleConduit::GetComplexTooltip()
{
	URuleNodeBase* StateNode = CastChecked<URuleNodeBase>(GraphNode);
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
