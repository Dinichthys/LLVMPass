#include "llvm/IR/IRBuilder.h"
#include "llvm/IR/Module.h"
#include "llvm/IR/Verifier.h"
#include "llvm/Passes/PassBuilder.h"

#include "pass/log_func.h"

namespace dinfo {

static bool IsFuncLogger(llvm::StringRef name);

static void InsertFuncLoggers(llvm::Module &M, llvm::Function &F, size_t func_idx,
                                llvm::IRBuilder<> &builder, llvm::LLVMContext &context);

static void FindCallAndInsertLog(llvm::Module &M, llvm::BasicBlock &B, size_t func_idx,
                                llvm::IRBuilder<> &builder, llvm::LLVMContext &context);

static void InsertFuncStartLog(llvm::Module &M, llvm::Function &F, size_t func_idx,
                                llvm::IRBuilder<> &builder, llvm::LLVMContext &context);
static void InsertBBStartLog(llvm::Module &M, llvm::BasicBlock &B, size_t bb_idx,
                                llvm::IRBuilder<> &builder, llvm::LLVMContext &context);
static void InsertCallLog(llvm::Module &M, llvm::BasicBlock &B, llvm::CallInst *C, size_t func_idx,
                                llvm::IRBuilder<> &builder, llvm::LLVMContext &context);

void InsertLoggers(llvm::Module &M) {
    llvm::LLVMContext &context = M.getContext();
    llvm::IRBuilder<> builder(context);

    size_t func_idx = 0;
    for (auto &F : M) {
        if (IsFuncLogger(F.getName()) || F.isDeclaration()) {
            continue;
        }

        InsertFuncLoggers(M, F, func_idx, builder, context);

        func_idx++;
    }
}

static bool IsFuncLogger(llvm::StringRef name) {
    return (name == kStartFuncLogFuncName) || (name == kStartBBLogFuncName) || (name == kCallFuncLogFuncName);
}

static void InsertFuncLoggers(llvm::Module &M, llvm::Function &F, size_t func_idx,
                                llvm::IRBuilder<> &builder, llvm::LLVMContext &context) {
    size_t bb_idx = 0;
    for (auto &B : F) {
        FindCallAndInsertLog(M, B, func_idx, builder, context);

        InsertBBStartLog(M, B, bb_idx, builder, context);
        bb_idx++;
    }

    InsertFuncStartLog(M, F, func_idx, builder, context);
}

static void FindCallAndInsertLog(llvm::Module &M, llvm::BasicBlock &B, size_t func_idx,
                                llvm::IRBuilder<> &builder, llvm::LLVMContext &context) {
    for (auto &I : B) {
        if (auto *C = llvm::dyn_cast<llvm::CallInst>(&I)) {
            if (IsFuncLogger(C->getCalledFunction()->getName())) {
                continue;
            }
            InsertCallLog(M, B, C, func_idx, builder, context);
        }
    }
}

static void InsertFuncStartLog(llvm::Module &M, llvm::Function &F, size_t func_idx,
                                llvm::IRBuilder<> &builder, llvm::LLVMContext &context) {
    llvm::Type *void_type = llvm::Type::getVoidTy(context);
    llvm::Type *int64_type = llvm::Type::getInt64Ty(context);

    llvm::ArrayRef<llvm::Type *> arg_types = {int64_type};
    llvm::FunctionType *log_func_type =
        llvm::FunctionType::get(void_type, arg_types, false);
    llvm::FunctionCallee log_func =
        M.getOrInsertFunction(kStartFuncLogFuncName, log_func_type);

    builder.SetInsertPoint(F.getEntryBlock().getFirstInsertionPt());
    llvm::Value *func_name = llvm::ConstantInt::get(int64_type, func_idx);
    llvm::Value *args[] = {func_name};
    builder.CreateCall(log_func, args);
}

static void InsertBBStartLog(llvm::Module &M, llvm::BasicBlock &B, size_t bb_idx,
                                llvm::IRBuilder<> &builder, llvm::LLVMContext &context) {
    llvm::Type *void_type = llvm::Type::getVoidTy(context);
    llvm::Type *int64_type = llvm::Type::getInt64Ty(context);

    llvm::ArrayRef<llvm::Type *> arg_types = {int64_type};
    llvm::FunctionType *log_func_type =
        llvm::FunctionType::get(void_type, arg_types, false);
    llvm::FunctionCallee log_func =
        M.getOrInsertFunction(kStartBBLogFuncName, log_func_type);

    builder.SetInsertPoint(B.getFirstInsertionPt());
    llvm::Value *bb_name = llvm::ConstantInt::get(int64_type, bb_idx);
    llvm::Value *args[] = {bb_name};
    builder.CreateCall(log_func, args);
}

static void InsertCallLog(llvm::Module &M, llvm::BasicBlock &B, llvm::CallInst *C, size_t func_idx,
                                llvm::IRBuilder<> &builder, llvm::LLVMContext &context) {
    llvm::Type *void_type = llvm::Type::getVoidTy(context);
    llvm::Type *int64_type = llvm::Type::getInt64Ty(context);

    llvm::ArrayRef<llvm::Type *> arg_types = {int64_type};
    llvm::FunctionType *log_func_type =
        llvm::FunctionType::get(void_type, arg_types, false);
    llvm::FunctionCallee log_func =
        M.getOrInsertFunction(kCallFuncLogFuncName, log_func_type);

    builder.SetInsertPoint(C);
    builder.SetInsertPoint(&B, ++builder.GetInsertPoint());
    llvm::Value *func_name = llvm::ConstantInt::get(int64_type, func_idx);
    llvm::Value *args[] = {func_name};
    builder.CreateCall(log_func, args);
}

}; // dinfo
