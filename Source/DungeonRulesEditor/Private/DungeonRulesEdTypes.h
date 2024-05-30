/*
 * MIT License
 *
 * Copyright (c) 2024 Benoit Pelletier
 *
 * Permission is hereby granted, free of charge, to any person obtaining a copy
 * of this software and associated documentation files (the "Software"), to deal
 * in the Software without restriction, including without limitation the rights
 * to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 * copies of the Software, and to permit persons to whom the Software is
 * furnished to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included in all
 * copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 * AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 * OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
 * SOFTWARE.
 */

#pragma once

#include "ProceduralDungeonTypes.h"
#include "UObject/NameTypes.h"
#include "Misc/EngineVersionComparison.h"

#if UE_VERSION_NEWER_THAN(5, 3, 0)
#define GC_PTR(VAR_TYPE) TObjectPtr<VAR_TYPE>
#else
#define GC_PTR(VAR_TYPE) VAR_TYPE*
#endif

#if UE_VERSION_OLDER_THAN(5, 1, 0)
using FStyleProvider = FEditorStyle;
#define STYLESET_NAME() FEditorStyle::GetStyleSetname()
#else
using FStyleProvider = FAppStyle;
#define STYLESET_NAME() FAppStyle::GetAppStyleSetName()
#endif

namespace DungeonRulesPinCategory
{
	const FName Transition(TEXT("Transition"));
	const FName Exec(TEXT("exec"));
}
