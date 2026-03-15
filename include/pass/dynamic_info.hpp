#pragma once

#include "llvm/IR/Module.h"

namespace dinfo {

void InsertLoggers(llvm::Module &M);

}; // namespace dinfo
