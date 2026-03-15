#include "llvm/IR/Module.h"
#include "llvm/Passes/PassBuilder.h"
#include "llvm/Passes/PassPlugin.h"

#include "pass/static_info.hpp"
#include "pass/dynamic_info.hpp"

struct GraphPass : public llvm::PassInfoMixin<GraphPass> {

    llvm::PreservedAnalyses run(llvm::Module &M, llvm::ModuleAnalysisManager &) {
        sinfo::PrintStaticInfo(M);
        dinfo::InsertLoggers(M);
        return llvm::PreservedAnalyses::none();
    };
};

llvm::PassPluginLibraryInfo getPassPluginInfo() {
    const auto callback = [](llvm::PassBuilder &PB) {
        PB.registerOptimizerLastEPCallback([](llvm::ModulePassManager &MPM, auto) {
            MPM.addPass(GraphPass{});
            return true;
        });
    };

    return {LLVM_PLUGIN_API_VERSION, "GraphCreator", "0.0.1", callback};
};

extern "C" LLVM_ATTRIBUTE_WEAK llvm::PassPluginLibraryInfo llvmGetPassPluginInfo() {
    return getPassPluginInfo();
}
