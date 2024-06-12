// Copyright (c) 2024 Benoit Pelletier
// SPDX-License-Identifier: BSL-1.0
// Distributed under the Boost Software License, Version 1.0. 
// (See accompanying file LICENSE or copy at https://www.boost.org/LICENSE_1_0.txt)

#include "DungeonRulesTypes.h"

#define LOCTEXT_NAMESPACE "DungeonRulesTypes"

namespace
{
	static constexpr uint8 Equal = 0;
	static constexpr uint8 Less = 1;
	static constexpr uint8 Greater = 2;
};

bool FComparisonHelper::Check(const int A, const int B, const EComparisonOp Operator)
{
#if false // naive approach
	switch (Comparison)
	{
	case ERoomCountComp::Equal:
		return Result == Count;
	case ERoomCountComp::NotEqual:
		return Result != Count;
	case ERoomCountComp::Greater:
		return Result > Count;
	case ERoomCountComp::GreaterEqual:
		return Result >= Count;
	case ERoomCountComp::Less:
		return Result < Count;
	case ERoomCountComp::LessEqual:
		return Result <= Count;
	default:
		checkNoEntry();
	}
#else // Using flags (no branching)
	uint8 Flags = ((A == B) << ::Equal)
				| ((A < B) << ::Less)
				| ((A > B) << ::Greater);
	return Flags & (uint8)Operator;
#endif
}

FText FComparisonHelper::GetComparisonText(const EComparisonOp Operator, const int Value)
{
	FText ValueText = FText::AsNumber(Value);
	switch (Operator)
	{
	case EComparisonOp::Equal:
		return FText::Format(LOCTEXT("Comparison_Equal", "exactly {0}"), ValueText);
	case EComparisonOp::NotEqual:
		return FText::Format(LOCTEXT("Comparison_NotEqual", "not {0}"), ValueText);
	case EComparisonOp::Greater:
		return FText::Format(LOCTEXT("Comparison_Greater", "more than {0}"), ValueText);
	case EComparisonOp::GreaterEqual:
		return FText::Format(LOCTEXT("Comparison_GreaterEqual", "{0} or more"), ValueText);
	case EComparisonOp::Less:
		return FText::Format(LOCTEXT("Comparison_Less", "less than {0}"), ValueText);
	case EComparisonOp::LessEqual:
		return FText::Format(LOCTEXT("Comparison_LessEqual", "{0} or less"), ValueText);
	default:
		checkNoEntry();
	}

	return FText();
}

#undef LOCTEXT_NAMESPACE
