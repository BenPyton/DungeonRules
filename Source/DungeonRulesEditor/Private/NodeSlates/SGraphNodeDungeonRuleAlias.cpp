// Copyright Epic Games, Inc. All Rights Reserved.

#include "SGraphNodeDungeonRuleAlias.h"

#include "Nodes/RuleAliasNode.h"
#include "Nodes/RuleNodeBase.h"
#include "Animation/AnimBlueprint.h"
#include "Animation/AnimBlueprintGeneratedClass.h"
#include "Animation/AnimInstance.h"
#include "DungeonRulesGraph.h"
#include "SGraphNodeDungeonRule.h"
#include "SGraphNodeDungeonRuleTransition.h"
#include "Containers/ContainerAllocationPolicies.h"
#include "Containers/Map.h"
#include "Containers/Set.h"
#include "EdGraph/EdGraphNode.h"
#include "Engine/Blueprint.h"
#include "HAL/PlatformCrt.h"
#include "Internationalization/Internationalization.h"
#include "Kismet2/BlueprintEditorUtils.h"
#include "Math/UnrealMathSSE.h"
#include "Styling/AppStyle.h"
#include "Templates/Casts.h"
#include "UObject/WeakObjectPtr.h"
#include "UObject/WeakObjectPtrTemplates.h"

class SToolTip;
struct FGraphInformationPopupInfo;
struct FNodeInfoContext;
struct FSlateBrush;

#define LOCTEXT_NAMESPACE "SGraphNodeDungeonRuleAlias"

/////////////////////////////////////////////////////
// SGraphNodeDungeonRuleAlias

void SGraphNodeDungeonRuleAlias::Construct(const FArguments& InArgs, URuleAliasNode* InNode)
{
	SGraphNodeDungeonRule::Construct(SGraphNodeDungeonRule::FArguments(), InNode);
}

#if false // Blueprint
void SGraphNodeDungeonRuleAlias::GetNodeInfoPopups(FNodeInfoContext* Context, TArray<FGraphInformationPopupInfo>& Popups) const
{
	GetStateInfoPopup(GraphNode, Popups);
}
#endif

FSlateColor SGraphNodeDungeonRuleAlias::GetBorderBackgroundColor_Internal(FLinearColor InactiveStateColor, FLinearColor ActiveStateColorDim, FLinearColor ActiveStateColorBright) const
{
#if false // Blueprint
	UAnimBlueprint* AnimBlueprint = Cast<UAnimBlueprint>(FBlueprintEditorUtils::FindBlueprintForNode(GraphNode));
	if (AnimBlueprint)
	{
		UAnimInstance* ActiveObject = Cast<UAnimInstance>(AnimBlueprint->GetObjectBeingDebugged());
		UAnimBlueprintGeneratedClass* Class = AnimBlueprint->GetAnimBlueprintGeneratedClass();

		// Display various types of debug data
		if ((ActiveObject != NULL) && (Class != NULL))
		{
			UDungeonRulesGraph* StateMachineGraph = CastChecked<UDungeonRulesGraph>(GraphNode->GetGraph());
			URuleAliasNode* StateAliasNode = CastChecked<URuleAliasNode>(GraphNode);

			if (FStateMachineDebugData* DebugInfo = Class->GetAnimBlueprintDebugData().StateMachineDebugData.Find(StateMachineGraph))
			{
				TArray<int32, TInlineAllocator<16>> StatesToCheck;

				// Check transitions associated with our state alias.
				TArray<FStateMachineDebugData::FStateAliasTransitionStateIndexPair> TransitionStatePairs;
				DebugInfo->StateAliasNodeToTransitionStatePairs.MultiFind(StateAliasNode, TransitionStatePairs);
				const int32 PairsNum = TransitionStatePairs.Num();
				for (int32 Index = 0; Index < PairsNum; ++Index)
				{
					auto& TransStatePair = TransitionStatePairs[Index];
					if (SGraphNodeDungeonRuleTransition::IsTransitionActive(TransStatePair.TransitionIndex, *Class, *StateMachineGraph, *ActiveObject))
					{
						// Add the prev/next state we are aliasing that's associated with this transition.
						StatesToCheck.Add(TransStatePair.AssociatedStateIndex);
					}
				}

				// Use the highest aliased state's weight that has an active transition in/out of this alias. 
				float Weight = 0.0f;
				for (const int32 StateIndex : StatesToCheck)
				{
					if (TWeakObjectPtr<URuleNodeBase>* StateNodeWeakPtr = DebugInfo->StateIndexToNode.Find(StateIndex))
					{
						if (StateAliasNode->GetAliasedStates().Contains(*StateNodeWeakPtr))
						{
							for (const FStateMachineStateDebugData& StateData : Class->GetAnimBlueprintDebugData().StateData)
							{
								if (StateData.StateMachineIndex == DebugInfo->MachineIndex && StateData.StateIndex == StateIndex)
								{
									Weight = FMath::Max(Weight, StateData.Weight);
								}
							}
						}
					}
				}

				if (Weight > 0.0f)
				{
					return FMath::Lerp<FLinearColor>(ActiveStateColorDim, ActiveStateColorBright, Weight);
				}
			}
		}
	}
#endif

	// Override inactive state color for aliases.
	return FLinearColor(0.45f, 0.33f, 0.37f);
}

FText SGraphNodeDungeonRuleAlias::GetPreviewCornerText() const
{
	return FText();
}

const FSlateBrush* SGraphNodeDungeonRuleAlias::GetNameIcon() const
{
	return FAppStyle::GetBrush(TEXT("Graph.AliasNode.Icon"));
}

TSharedPtr<SToolTip> SGraphNodeDungeonRuleAlias::GetComplexTooltip()
{
	return nullptr;
}

#if false // Blueprint
void SGraphNodeDungeonRuleAlias::GetStateInfoPopup(UEdGraphNode* GraphNode, TArray<FGraphInformationPopupInfo>& Popups)
{
	UAnimBlueprint* AnimBlueprint = Cast<UAnimBlueprint>(FBlueprintEditorUtils::FindBlueprintForNode(GraphNode));
	if (AnimBlueprint)
	{
		UAnimInstance* ActiveObject = Cast<UAnimInstance>(AnimBlueprint->GetObjectBeingDebugged());
		UAnimBlueprintGeneratedClass* Class = AnimBlueprint->GetAnimBlueprintGeneratedClass();

		static const FLinearColor PopupColor(1.f, 0.5f, 0.25f);

		// Display various types of debug data
		if ((ActiveObject != NULL) && (Class != NULL))
		{
			UDungeonRulesGraph* StateMachineGraph = CastChecked<UDungeonRulesGraph>(GraphNode->GetGraph());
			URuleAliasNode* StateAliasNode = CastChecked<URuleAliasNode>(GraphNode);

			if (FStateMachineDebugData* DebugInfo = Class->GetAnimBlueprintDebugData().StateMachineDebugData.Find(StateMachineGraph))
			{
				TArray<int32, TInlineAllocator<16>> StatesToCheck;

				// Check transitions associated with our state alias.
				TArray<FStateMachineDebugData::FStateAliasTransitionStateIndexPair> TransitionStatePairs;
				DebugInfo->StateAliasNodeToTransitionStatePairs.MultiFind(StateAliasNode, TransitionStatePairs);
				const int32 PairsNum = TransitionStatePairs.Num();
				for (int32 Index = 0; Index < PairsNum; ++Index)
				{
					auto& TransStatePair = TransitionStatePairs[Index];
					if (SGraphNodeDungeonRuleTransition::IsTransitionActive(TransStatePair.TransitionIndex, *Class, *StateMachineGraph, *ActiveObject))
					{
						// Add the prev/next state we are aliasing that's associated with this transition.
						StatesToCheck.Add(TransStatePair.AssociatedStateIndex);
					}
				}

				// Display the name and weight of any state we are aliasing that has an active incoming/outgoing transition from this alias
				for (const int32 StateIndex : StatesToCheck)
				{
					if (TWeakObjectPtr<URuleNodeBase>* StateNodeWeakPtr = DebugInfo->StateIndexToNode.Find(StateIndex))
					{
						if (URuleNodeBase* StateNode = StateNodeWeakPtr->Get())
						{
							if (StateAliasNode->bGlobalAlias || StateAliasNode->GetAliasedStates().Contains(StateNode))
							{
								for (const FStateMachineStateDebugData& StateData : Class->GetAnimBlueprintDebugData().StateData)
								{
									if (StateData.StateMachineIndex == DebugInfo->MachineIndex && StateData.StateIndex == StateIndex)
									{
										FText StateText;
										StateText = FText::Format(LOCTEXT("ActiveAliasedStateFormat", "{0}(Alias) {1}\nActive for {2}s"),
											FText::FromString(StateNode->GetStateName()),
											FText::AsPercent(StateData.Weight),
											FText::AsNumber(StateData.ElapsedTime));
										Popups.Emplace(nullptr, PopupColor, StateText.ToString());
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
}
#endif

#undef LOCTEXT_NAMESPACE
