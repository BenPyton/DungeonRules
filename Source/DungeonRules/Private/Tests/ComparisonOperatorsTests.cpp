// Copyright (c) 2024 Benoit Pelletier
// SPDX-License-Identifier: BSL-1.0
// Distributed under the Boost Software License, Version 1.0. 
// (See accompanying file LICENSE or copy at https://www.boost.org/LICENSE_1_0.txt)

#include "CoreTypes.h"
#include "Misc/AutomationTest.h"
#include "DungeonRulesTypes.h"

#if WITH_DEV_AUTOMATION_TESTS

IMPLEMENT_SIMPLE_AUTOMATION_TEST(FComparisonOperatorsTests, "ProceduralDungeon.Rules.ComparisonOperators", EAutomationTestFlags::ApplicationContextMask | EAutomationTestFlags::SmokeFilter)

bool FComparisonOperatorsTests::RunTest(const FString& Parameters)
{
	{
		TestTrue(TEXT("0 == 0"), FComparisonHelper::Check(0, 0, EComparisonOp::Equal));
		TestFalse(TEXT("0 == 1"), FComparisonHelper::Check(0, 1, EComparisonOp::Equal));
		TestFalse(TEXT("1 == 0"), FComparisonHelper::Check(1, 0, EComparisonOp::Equal));

		TestFalse(TEXT("0 != 0"), FComparisonHelper::Check(0, 0, EComparisonOp::NotEqual));
		TestTrue(TEXT("0 != 1"), FComparisonHelper::Check(0, 1, EComparisonOp::NotEqual));
		TestTrue(TEXT("1 != 0"), FComparisonHelper::Check(1, 0, EComparisonOp::NotEqual));

		TestFalse(TEXT("0 < 0"), FComparisonHelper::Check(0, 0, EComparisonOp::Less));
		TestTrue(TEXT("0 < 1"), FComparisonHelper::Check(0, 1, EComparisonOp::Less));
		TestFalse(TEXT("1 < 0"), FComparisonHelper::Check(1, 0, EComparisonOp::Less));

		TestTrue(TEXT("0 <= 0"), FComparisonHelper::Check(0, 0, EComparisonOp::LessEqual));
		TestTrue(TEXT("0 <= 1"), FComparisonHelper::Check(0, 1, EComparisonOp::LessEqual));
		TestFalse(TEXT("1 <= 0"), FComparisonHelper::Check(1, 0, EComparisonOp::LessEqual));

		TestFalse(TEXT("0 > 0"), FComparisonHelper::Check(0, 0, EComparisonOp::Greater));
		TestFalse(TEXT("0 > 1"), FComparisonHelper::Check(0, 1, EComparisonOp::Greater));
		TestTrue(TEXT("1 > 0"), FComparisonHelper::Check(1, 0, EComparisonOp::Greater));

		TestTrue(TEXT("0 >= 0"), FComparisonHelper::Check(0, 0, EComparisonOp::GreaterEqual));
		TestFalse(TEXT("0 >= 1"), FComparisonHelper::Check(0, 1, EComparisonOp::GreaterEqual));
		TestTrue(TEXT("1 >= 0"), FComparisonHelper::Check(1, 0, EComparisonOp::GreaterEqual));
	}

	return true;
}

#endif //WITH_DEV_AUTOMATION_TESTS
