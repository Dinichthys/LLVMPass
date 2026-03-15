#include "pass/static_info.hpp"
#include "pass/static_info_constants.hpp"

#include "llvm/IR/Module.h"
#include "llvm/Demangle/Demangle.h"
#include "llvm/IR/Instructions.h"
#include "llvm/IR/Constants.h"

namespace sinfo {

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

static void PrintClusterStart(const std::string_view &cluster_name, std::function<void(void)> PrintLabel,
                            const std::string_view &cluster_color, const std::string_view &cluster_border_color,
                            llvm::raw_fd_ostream &out);
static void PrintClusterEnd(llvm::raw_fd_ostream &out);

static void PrintModuleClusterBody(llvm::Module &M, llvm::raw_fd_ostream &out);
static void PrintFuncClusterBody(size_t func_idx, llvm::Function &F, llvm::raw_fd_ostream &out);
static void PrintBasicBlockClusterNodes(llvm::BasicBlock &B, llvm::raw_fd_ostream &out);
static void PrintBasicBlockClusterEdges(llvm::BasicBlock &B, llvm::raw_fd_ostream &out);

static void AddNode(const std::string_view &node_name, const std::function<void(void)> PrintLabel,
                const std::string_view &node_color, llvm::raw_fd_ostream &out);
static void AddEdge(const std::string_view &start_node_name, const std::string_view &end_node_name,
                    const std::string_view &edge_color, llvm::raw_fd_ostream &out);

void PrintStaticInfo(llvm::Module &M) {
    std::error_code EC = {};
    llvm::raw_fd_ostream out(kStaticInfoFileName, EC);
    if (EC) {
        llvm::errs() << "Can't open file " << EC.message() << "\n";
        return;
    }
    out.SetUnbuffered();

    llvm::raw_fd_ostream *out_ptr = &out;

    llvm::Module *module_ptr = &M;
    PrintClusterStart(kModuleClusterPrefixName, [module_ptr, out_ptr](){*out_ptr << module_ptr->getModuleIdentifier();},
                    kModuleClusterColor, kClusterBorderColor, out);
    PrintModuleClusterBody(M, out);
    PrintClusterEnd(out);
}

static void PrintClusterStart(const std::string_view &cluster_name, std::function<void(void)> PrintLabel,
                            const std::string_view &cluster_color, const std::string_view &cluster_border_color,
                            llvm::raw_fd_ostream &out) {
    out << kKeyWordStartClusterBlock << "\n";
    out << "cluster_" << cluster_name << "\n"
        << cluster_color << "\n";
    PrintLabel();
    out << "\n" << cluster_border_color << "\n";
}

static void PrintClusterEnd(llvm::raw_fd_ostream &out) {
    out << kKeyWordEndBlock << "\n";
}

static void PrintModuleClusterBody(llvm::Module &M, llvm::raw_fd_ostream &out) {
    size_t func_idx = 0;
    llvm::raw_fd_ostream *out_ptr = &out;
    for (auto &F : M) {
        llvm::Function *func_ptr = &F;
        if (F.isDeclaration()) {
            AddNode(std::to_string(size_t(&F)), [func_ptr, out_ptr](){*out_ptr << llvm::demangle(func_ptr->getName());},
                    kFuncDeclarationNodeColor, out);
            continue;
        }

        PrintClusterStart(kModuleClusterPrefixName + kFuncClusterPrefixName + std::to_string(func_idx),
                        [func_ptr, out_ptr](){*out_ptr << llvm::demangle(func_ptr->getName());},
                        kFuncClusterColor, kClusterBorderColor, out);
        PrintFuncClusterBody(func_idx, F, out);
        PrintClusterEnd(out);
        func_idx++;
    }
}

static void PrintFuncClusterBody(size_t func_idx, llvm::Function &F, llvm::raw_fd_ostream &out) {
    size_t bb_idx = 0;
    llvm::raw_fd_ostream *out_ptr = &out;
    for (auto &B : F) {
        llvm::BasicBlock *bb_ptr = &B;
        PrintClusterStart(kModuleClusterPrefixName
                        + kFuncClusterPrefixName + std::to_string(func_idx)
                        + kBasicBlockClusterPrefixName + std::to_string(bb_idx),
                        [bb_ptr, out_ptr](){
                                    *out_ptr << kBasicBlockClusterPrefixName;
                                    bb_ptr->printAsOperand(*out_ptr);},
                        kBasicBlockClusterColor, kClusterBorderColor, out);
        PrintBasicBlockClusterNodes(B, out);
        PrintClusterEnd(out);
        bb_idx++;
    }
    for (auto &B : F) {
        PrintBasicBlockClusterEdges(B, out);
    }
}

static void PrintBasicBlockClusterNodes(llvm::BasicBlock &B, llvm::raw_fd_ostream &out) {
    llvm::raw_fd_ostream *out_ptr = &out;
    for (auto &I : B) {
        llvm::Instruction *inst_ptr = &I;
        AddNode(std::to_string(size_t(&I)), [inst_ptr, out_ptr](){*out_ptr << inst_ptr->getOpcodeName();},
                kDefaultNodeFillColor, out);

        for (auto &U : I.operands()) {
            llvm::Value *use = U.get();
            llvm::Instruction* instr = nullptr;
            instr = llvm::dyn_cast<llvm::Instruction>(use);
            if (instr != nullptr) {
                continue;
            }

            AddNode(std::to_string(size_t(&U)), [use, out_ptr](){
                use->getType()->print(*out_ptr);

                *out_ptr << " ";

                if (auto *const_int = llvm::dyn_cast<llvm::ConstantInt>(use)) {
                    const_int->getValue().print(*out_ptr, true);
                } else {
                    use->printAsOperand(*out_ptr, false);
                }
            }, kExtraNodeFillColor, out);
        }
    }
}

static void PrintBasicBlockClusterEdges(llvm::BasicBlock &B, llvm::raw_fd_ostream &out) {
    llvm::Instruction* prev_i = &(B.front());
    size_t i_idx = 0;
    for (auto &I : B) {
        if (i_idx != 0) {
            AddEdge(std::to_string(size_t(prev_i)), std::to_string(size_t(&I)),
                    kCFGEdgeColor, out);
            prev_i = &I;
        }
        i_idx++;

        for (auto &U : I.uses()) {
            AddEdge(std::to_string(size_t(&I)), std::to_string(size_t(U.getUser())),
                    kDFGEdgeColor, out);
        }

        for (auto &U : I.operands()) {
            llvm::Value *use = U.get();
            llvm::Instruction* instr = nullptr;
            instr = llvm::dyn_cast<llvm::Instruction>(use);
            if (instr != nullptr) {
                continue;
            }

            AddEdge(std::to_string(size_t(&U)), std::to_string(size_t(&I)), kExtraEdgeColor, out);
        }

        if (auto *Br = llvm::dyn_cast<llvm::BranchInst>(&I)) {
            size_t suc_num = Br->getNumSuccessors();
            for (size_t suc_bb_idx = 0; suc_bb_idx < suc_num; suc_bb_idx++) {
                auto *suc_bb = Br->getSuccessor(suc_bb_idx);
                AddEdge(std::to_string(size_t(&I)), std::to_string(size_t(&(suc_bb->front()))), kCFGEdgeColor, out);
            }
        } else if (auto *S = llvm::dyn_cast<llvm::SwitchInst>(&I)) {
            size_t suc_num = S->getNumSuccessors();
            for (size_t suc_bb_idx = 0; suc_bb_idx < suc_num; suc_bb_idx++) {
                auto *suc_bb = S->getSuccessor(suc_bb_idx);
                AddEdge(std::to_string(size_t(&I)), std::to_string(size_t(&(suc_bb->front()))), kCFGEdgeColor, out);
            }
        } else if (auto *C = llvm::dyn_cast<llvm::CallBase>(&I)) {
            llvm::Function *func_ptr = C->getCalledFunction();
            if (llvm::Function *func = llvm::dyn_cast<llvm::Function>(func_ptr)) {
                if (func->isDeclaration()) {
                    AddEdge(std::to_string(size_t(&I)), std::to_string(size_t(func)), kCallEdgeColor, out);
                } else {
                    AddEdge(std::to_string(size_t(&I)), std::to_string(size_t(&(func->front().front()))), kCallEdgeColor, out);
                }
            }
        }
    }
}

static void AddNode(const std::string_view &node_name, const std::function<void(void)> PrintLabel,
                const std::string_view &node_color, llvm::raw_fd_ostream &out) {
    out << kKeyWordStartNodeBlock << "\n";
    out << kNodePrefixName << node_name << "\n";
    PrintLabel();
    out << "\n" << node_color << "\n"
        << kNodeBorderColor << "\n";
}

static void AddEdge(const std::string_view &start_node_name, const std::string_view &end_node_name,
            const std::string_view &edge_color, llvm::raw_fd_ostream &out) {
    out << kKeyWordStartEdgeBlock << "\n";
    out << kNodePrefixName << start_node_name << "\n"
        << kNodePrefixName << end_node_name << "\n"
        << edge_color << "\n";
}

}; // sinfo
