#include "llvm/IR/Module.h"
#include "llvm/Demangle/Demangle.h"
#include "llvm/Passes/PassBuilder.h"
#include "llvm/Passes/PassPlugin.h"
using namespace llvm;

static const std::string kModuleClusterPrefixName = "module_";
static const std::string kModuleClusterColor = "#E0E0E0";
static const std::string kFuncClusterPrefixName = "func_";
static const std::string kFuncClusterColor = "#D0D0D0";
static const std::string kFuncDeclarationNodeColor = "#E0E080";
static const std::string kBasicBlockClusterPrefixName = "basicblock_";
static const std::string kBasicBlockClusterColor = "#C0C0C0";
static const std::string kClusterBorderColor = "blue";
static const std::string kNodePrefixName = "node_";
static const std::string kDefaultNodeFillColor = "#90C0F0";
static const std::string kExtraNodeFillColor = "#90F0C0";
static const std::string kNodeBorderColor = "black";
static const std::string kDFGEdgeColor = "black";
static const std::string kCFGEdgeColor = "red";
static const std::string kCallEdgeColor = "blue";
static const std::string kExtraEdgeColor = "green";
static const std::string kEdgeWeight = std::to_string(10);

void PrintStartGraph() {
    outs() << "digraph\n{\n"
            "\tfontname = \"Helvetica,Arial,sans-serif\";\n"
            "\tnode [fontname = \"Helvetica,Arial,sans-serif\"];\n"
            "\tgraph [rankdir = \"TB\"];\n"
            "\tranksep = 1.5;\n"
            "\tsplines = ortho;\n\n";
};

void PrintEndGraph() {
    outs() << "}\n";
};

static size_t ModuleIdx = 0;
struct MyModPassStart : public PassInfoMixin<MyModPassStart> {

    PreservedAnalyses run(Module &M, ModuleAnalysisManager &) {
        if (ModuleIdx == 0) {
            PrintStartGraph();
        }

        PrintClusterStart(kModuleClusterPrefixName + std::to_string(ModuleIdx), M.getModuleIdentifier(),
                        kModuleClusterColor, kClusterBorderColor);
        PrintModuleClusterBody(ModuleIdx, M);
        PrintClusterEnd();
        ModuleIdx++;

        PrintEndGraph();

        return PreservedAnalyses::all();
    };

    void PrintClusterStart(const std::string_view &cluster_name, const std::string_view &cluster_label,
                            const std::string_view &cluster_color, const std::string_view &cluster_border_color) {
        outs() << "\tsubgraph cluster_" << cluster_name << " {\n"
                "\t\tnode [style=filled];\n"
                "\t\tstyle=\"filled\";\n"
                "\t\tfillcolor=\"" << cluster_color << "\";\n"
                "\t\tlabel = \"" << cluster_label << "\";\n"
                "\t\tcolor=\"" << cluster_border_color << "\";\n";
    };

    void PrintClusterEnd() {
        outs() << "\t}\n";
    };

    void PrintModuleClusterBody(size_t module_idx, Module &M) {
        size_t func_idx = 0;
        for (auto &F : M) {
            if (F.isDeclaration()) {
                Function *func_ptr = &F;
                AddNode(std::to_string(size_t(&F)), [func_ptr](){outs() << demangle(func_ptr->getName());},
                        kFuncDeclarationNodeColor);
                continue;
            }

            PrintClusterStart(kModuleClusterPrefixName + std::to_string(module_idx)
                            + kFuncClusterPrefixName + std::to_string(func_idx), demangle(F.getName()),
                            kFuncClusterColor, kClusterBorderColor);
            PrintFuncClusterBody(module_idx, func_idx, F);
            PrintClusterEnd();
            func_idx++;
        }
    };

    void PrintFuncClusterBody(size_t module_idx, size_t func_idx, Function &F) {
        size_t bb_idx = 0;
        for (auto &B : F) {
            PrintClusterStart(kModuleClusterPrefixName + std::to_string(module_idx)
                            + kFuncClusterPrefixName + std::to_string(func_idx)
                            + kBasicBlockClusterPrefixName + std::to_string(bb_idx),
                            kBasicBlockClusterPrefixName + std::to_string(bb_idx),
                            kBasicBlockClusterColor, kClusterBorderColor);
            PrintBasicBlockClusterNodes(B);
            PrintClusterEnd();
            bb_idx++;
        }
        for (auto &B : F) {
            PrintBasicBlockClusterEdges(B);
            bb_idx++;
        }
    };

    void PrintBasicBlockClusterNodes(BasicBlock &B) {
        size_t i_idx = 0;
        for (auto &I : B) {
            Instruction *inst_ptr = &I;
            AddNode(std::to_string(size_t(&I)), [inst_ptr](){outs() << inst_ptr->getOpcodeName();},
                    kDefaultNodeFillColor);

            for (auto &U : I.operands()) {
                Value *use = U.get();
                Instruction* instr = nullptr;
                instr = dyn_cast<Instruction, Value>(use);
                if (instr != nullptr) {
                    continue;
                }

                AddNode(std::to_string(size_t(&U)), [use](){
                    use->getType()->print(outs());

                    outs() << " ";

                    if (auto *const_int = dyn_cast<ConstantInt>(use)) {
                        const_int->getValue().print(outs(), true);
                    }
                }, kExtraNodeFillColor);
            }
        }
    };

    void PrintBasicBlockClusterEdges(BasicBlock &B) {
        Instruction* prev_i = &(B.front());
        size_t i_idx = 0;
        for (auto &I : B) {
            if (i_idx != 0) {
                AddEdge(std::to_string(size_t(prev_i)), std::to_string(size_t(&I)),
                        kCFGEdgeColor);
                prev_i = &I;
            }
            i_idx++;

            for (auto &U : I.uses()) {
                AddEdge(std::to_string(size_t(&I)), std::to_string(size_t(U.getUser())),
                        kDFGEdgeColor);
            }

            for (auto &U : I.operands()) {
                Value *use = U.get();
                Instruction* instr = nullptr;
                instr = dyn_cast<Instruction, Value>(use);
                if (instr != nullptr) {
                    continue;
                }

                AddEdge(std::to_string(size_t(&U)), std::to_string(size_t(&I)), kExtraEdgeColor);
            }

            if (auto *Br = dyn_cast<BranchInst>(&I)) {
                size_t suc_num = Br->getNumSuccessors();
                for (size_t suc_bb_idx = 0; suc_bb_idx < suc_num; suc_bb_idx++) {
                    auto *suc_bb = Br->getSuccessor(suc_bb_idx);
                    AddEdge(std::to_string(size_t(&I)), std::to_string(size_t(&(suc_bb->front()))), kCFGEdgeColor);
                }
            } else if (auto *S = dyn_cast<SwitchInst>(&I)) {
                size_t suc_num = S->getNumSuccessors();
                for (size_t suc_bb_idx = 0; suc_bb_idx < suc_num; suc_bb_idx++) {
                    auto *suc_bb = S->getSuccessor(suc_bb_idx);
                    AddEdge(std::to_string(size_t(&I)), std::to_string(size_t(&(suc_bb->front()))), kCFGEdgeColor);
                }
            } else if (auto *C = dyn_cast<CallBase>(&I)) {
                Function *func_ptr = C->getCalledFunction();
                if (Function *func = dyn_cast<Function>(func_ptr)) {
                    if (&(func->front().front()) != nullptr) {
                        AddEdge(std::to_string(size_t(&I)), std::to_string(size_t(&(func->front().front()))), kCallEdgeColor);
                    } else if (func->isDeclaration()) {
                        AddEdge(std::to_string(size_t(&I)), std::to_string(size_t(func)), kCallEdgeColor);
                    }
                }
            }
        }
    };

    void AddNode(const std::string_view &node_name, const std::function<void(void)> PrintLabel,
                 const std::string_view &node_color) {
        outs() << "\t\"" << kNodePrefixName << node_name << "\"\n\t[\n"
                    "\t\tlabel = \"";
        PrintLabel();
        outs() << "\"\n"
                    "\t\tshape = \"record\"\n"
                    "\t\tstyle = \"filled\"\n"
                    "\t\tfillcolor = \"" << node_color << "\"\n"
                    "\t\tcolor = \"" << kNodeBorderColor << "\"\n"
                    "\t];\n\n";
    };

    void AddEdge(const std::string_view &start_node_name, const std::string_view &end_node_name,
                const std::string_view &edge_color) {
        outs() << "\t\"" << kNodePrefixName << start_node_name << "\" -> \""
                << kNodePrefixName << end_node_name << "\""
                "[color = \"" << edge_color << "\", weight = " << kEdgeWeight << "]\n";
    };
};

// size_t MyModPassStart::ModuleIdx = 0;

struct MyModPassEnd : public PassInfoMixin<MyModPassEnd> {
    PreservedAnalyses run(Module &, ModuleAnalysisManager &) {
        static bool first_mod = true;
        if (first_mod) {
            PrintEndGraph();
            first_mod = false;
        }
        return PreservedAnalyses::all();
    };
};

PassPluginLibraryInfo getPassPluginInfo() {
    const auto callback = [](PassBuilder &PB) {
        // PB.registerPipelineStartEPCallback([](ModulePassManager &MPM, auto) {
        // MPM.addPass(MyModPassStart{});
        // return true;
        // });
        PB.registerOptimizerLastEPCallback([](ModulePassManager &MPM, auto) {
        MPM.addPass(MyModPassStart{});
        return true;
        });
    };

    return {LLVM_PLUGIN_API_VERSION, "MyPlugin", "0.0.1", callback};
};

/* When a plugin is loaded by the driver, it will call this entry point to
obtain information about this plugin and about how to register its passes.
*/
extern "C" LLVM_ATTRIBUTE_WEAK PassPluginLibraryInfo llvmGetPassPluginInfo() {
    return getPassPluginInfo();
}
