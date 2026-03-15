#include "pass/log_func.h"

#include <stdlib.h>
#include <stdio.h>
#include <stdbool.h>

FILE* LogFile = NULL;
bool TriedOpenFile = false;

static void OpenLogFile();

void StartFuncLog(size_t func_idx) {
    if (LogFile == NULL) {
        if (!TriedOpenFile) {
            OpenLogFile();
        }
        if (LogFile == NULL) {
            fprintf(stderr, "%s %lu\n", kFuncEnterKeyWord, func_idx);
        } else {
            fprintf(LogFile, "%s %lu\n", kFuncEnterKeyWord, func_idx);
        }
        return;
    }
    fprintf(LogFile, "%s %lu\n", kFuncEnterKeyWord, func_idx);
}

void StartBBLog(size_t bb_idx) {
    if (LogFile == NULL) {
        if (!TriedOpenFile) {
            OpenLogFile();
        }
        if (LogFile == NULL) {
            fprintf(stderr, "%s %lu\n", kBBEnterKeyWord, bb_idx);
        } else {
            fprintf(LogFile, "%s %lu\n", kBBEnterKeyWord, bb_idx);
        }
        return;
    }
    fprintf(LogFile, "%s %lu\n", kBBEnterKeyWord, bb_idx);
}

static void OpenLogFile() {
    LogFile = fopen(kDynamicInfoFileName, "w");
    TriedOpenFile = true;
    if (LogFile != NULL) {
        setbuf(LogFile, NULL);
    }
}
