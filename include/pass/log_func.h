#pragma once

#include <stdlib.h>

#define WORD_TO_STR(x) #x

void StartFuncLog(size_t func_idx);
void StartBBLog(size_t bb_idx);
void CallFuncLog(size_t func_idx);

static const char* const kStartFuncLogFuncName = WORD_TO_STR(StartFuncLog);
static const char* const kStartBBLogFuncName = WORD_TO_STR(StartBBLog);
static const char* const kCallFuncLogFuncName = WORD_TO_STR(CallFuncLog);

#undef WORD_TO_STR

static const char* const kFuncEnterKeyWord = "FUNC";
static const char* const kBBEnterKeyWord = "BASICBLOCK";
static const char* const kCallFuncKeyWord = "CALL";

static const char* const kDynamicInfoFileName = "log/dynamic.tmp.log";
