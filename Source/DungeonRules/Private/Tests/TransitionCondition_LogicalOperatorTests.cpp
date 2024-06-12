// Copyright (c) 2024 Benoit Pelletier
// SPDX-License-Identifier: BSL-1.0
// Distributed under the Boost Software License, Version 1.0. 
// (See accompanying file LICENSE or copy at https://www.boost.org/LICENSE_1_0.txt)

#include "CoreTypes.h"
#include "Misc/AutomationTest.h"
#include "TransitionConditions/DRT_LogicalOperator.h"
#include "TransitionConditions/DRT_NotOperator.h"
#include "UObject/StrongObjectPtr.h"
#include "TransitionConditionTestClasses.h"

#if WITH_DEV_AUTOMATION_TESTS

IMPLEMENT_SIMPLE_AUTOMATION_TEST(FTransitionCondition_LogicalOperatorTests, "ProceduralDungeon.Rules.LogicalOperators", EAutomationTestFlags::ApplicationContextMask | EAutomationTestFlags::EngineFilter)

namespace {
	using ConditionList = TArray<TObjectPtr<URuleTransitionCondition>>;
}

bool FTransitionCondition_LogicalOperatorTests::RunTest(const FString& Parameters)
{
	// Creating condition instances
	CREATE_CONDITION_INSTANCE(UDRT_True, TRUE);
	CREATE_CONDITION_INSTANCE(UDRT_False, FALSE);

	::ConditionList Empty = {};
	::ConditionList OneFalse = {FALSE.Get()};
	::ConditionList OneTrue = {TRUE.Get()};
	::ConditionList AllFalse = {FALSE.Get(), FALSE.Get(), FALSE.Get()};
	::ConditionList AllTrue = {TRUE.Get(), TRUE.Get(), TRUE.Get()};
	::ConditionList FalseTrueTrue = {FALSE.Get(), TRUE.Get(), TRUE.Get()};
	::ConditionList TrueFalseTrue = {TRUE.Get(), FALSE.Get(), TRUE.Get()};
	::ConditionList TrueTrueFalse = {TRUE.Get(), TRUE.Get(), FALSE.Get()};
	::ConditionList TrueFalseFalse = {TRUE.Get(), FALSE.Get(), FALSE.Get()};
	::ConditionList FalseTrueFalse = {FALSE.Get(), TRUE.Get(), FALSE.Get()};
	::ConditionList FalseFalseTrue = {FALSE.Get(), FALSE.Get(), TRUE.Get()};

	// Test AND
	{
		CREATE_CONDITION_INSTANCE(UDRT_LogicalOperator, Operator_AND);
		Operator_AND->SetOperator(ELogicalOperator::AND);

		Operator_AND->SetConditions(Empty);
		TestTrue(TEXT("[AND] {}"), Operator_AND->Check(nullptr, nullptr));

		Operator_AND->SetConditions(OneFalse);
		TestFalse(TEXT("[AND] {false}"), Operator_AND->Check(nullptr, nullptr));

		Operator_AND->SetConditions(OneTrue);
		TestTrue(TEXT("[AND] {true}"), Operator_AND->Check(nullptr, nullptr));

		Operator_AND->SetConditions(AllFalse);
		TestFalse(TEXT("[AND] {false, false, false}"), Operator_AND->Check(nullptr, nullptr));

		Operator_AND->SetConditions(AllTrue);
		TestTrue(TEXT("[AND] {true, true, true}"), Operator_AND->Check(nullptr, nullptr));

		Operator_AND->SetConditions(FalseTrueTrue);
		TestFalse(TEXT("[AND] {false, true, true}"), Operator_AND->Check(nullptr, nullptr));

		Operator_AND->SetConditions(TrueFalseTrue);
		TestFalse(TEXT("[AND] {true, false, true}"), Operator_AND->Check(nullptr, nullptr));

		Operator_AND->SetConditions(TrueTrueFalse);
		TestFalse(TEXT("[AND] {true, true, false}"), Operator_AND->Check(nullptr, nullptr));

		Operator_AND->SetConditions(TrueFalseFalse);
		TestFalse(TEXT("[AND] {true, false, false}"), Operator_AND->Check(nullptr, nullptr));

		Operator_AND->SetConditions(FalseTrueFalse);
		TestFalse(TEXT("[AND] {false, true, false}"), Operator_AND->Check(nullptr, nullptr));

		Operator_AND->SetConditions(FalseFalseTrue);
		TestFalse(TEXT("[AND] {false, false, true}"), Operator_AND->Check(nullptr, nullptr));
	}

	// Test OR
	{
		CREATE_CONDITION_INSTANCE(UDRT_LogicalOperator, Operator_OR);
		Operator_OR->SetOperator(ELogicalOperator::OR);

		Operator_OR->SetConditions(Empty);
		TestTrue(TEXT("[OR] {}"), Operator_OR->Check(nullptr, nullptr));

		Operator_OR->SetConditions(OneFalse);
		TestFalse(TEXT("[OR] {false}"), Operator_OR->Check(nullptr, nullptr));

		Operator_OR->SetConditions(OneTrue);
		TestTrue(TEXT("[OR] {true}"), Operator_OR->Check(nullptr, nullptr));

		Operator_OR->SetConditions(AllFalse);
		TestFalse(TEXT("[OR] {false, false, false}"), Operator_OR->Check(nullptr, nullptr));

		Operator_OR->SetConditions(AllTrue);
		TestTrue(TEXT("[OR] {true, true, true}"), Operator_OR->Check(nullptr, nullptr));

		Operator_OR->SetConditions(FalseTrueTrue);
		TestTrue(TEXT("[OR] {false, true, true}"), Operator_OR->Check(nullptr, nullptr));

		Operator_OR->SetConditions(TrueFalseTrue);
		TestTrue(TEXT("[OR] {true, false, true}"), Operator_OR->Check(nullptr, nullptr));

		Operator_OR->SetConditions(TrueTrueFalse);
		TestTrue(TEXT("[OR] {true, true, false}"), Operator_OR->Check(nullptr, nullptr));

		Operator_OR->SetConditions(TrueFalseFalse);
		TestTrue(TEXT("[OR] {true, false, false}"), Operator_OR->Check(nullptr, nullptr));

		Operator_OR->SetConditions(FalseTrueFalse);
		TestTrue(TEXT("[OR] {false, true, false}"), Operator_OR->Check(nullptr, nullptr));

		Operator_OR->SetConditions(FalseFalseTrue);
		TestTrue(TEXT("[OR] {false, false, true}"), Operator_OR->Check(nullptr, nullptr));
	}

	// Test NOT
	{
		CREATE_CONDITION_INSTANCE(UDRT_NotOperator, Operator_NOT);

		Operator_NOT->SetCondition(nullptr);
		TestFalse(TEXT("[NOT] null"), Operator_NOT->Check(nullptr, nullptr));

		Operator_NOT->SetCondition(FALSE.Get());
		TestTrue(TEXT("[NOT] false"), Operator_NOT->Check(nullptr, nullptr));

		Operator_NOT->SetCondition(TRUE.Get());
		TestFalse(TEXT("[NOT] true"), Operator_NOT->Check(nullptr, nullptr));
	}

	return true;
}

#endif //WITH_DEV_AUTOMATION_TESTS
