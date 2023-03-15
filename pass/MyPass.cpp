#include "llvm/IR/Function.h"
#include "llvm/IR/IRBuilder.h"
#include "llvm/IR/InstrTypes.h"
#include "llvm/IR/LegacyPassManager.h"
#include "llvm/IR/Module.h"
#include "llvm/Pass.h"
#include "llvm/Support/raw_ostream.h"
#include "llvm/Transforms/IPO/PassManagerBuilder.h"
#include "llvm/Transforms/Utils/BasicBlockUtils.h"

const char tmp_path[] = "./graphs/static_tmp.txt";
const char graph_path[] = "./graphs/static.txt";

using namespace llvm;

namespace {

// Define your Pass as a subclass of one of the existing pass interfaces.
struct MyPass : public FunctionPass {
  static char ID;
  MyPass() : FunctionPass(ID), output_(nullptr), depth_(0), is_inspected_(false) {
    output_ = fopen(tmp_path, "w");
    assert(output_);

    PrintNTabs();
    fprintf(output_, "digraph {\n");
    depth_++;

    PrintNTabs();
    fprintf(output_, "compound=true;\n");
  }

  ~MyPass() {
    depth_--;
    PrintNTabs();
    fprintf(output_, "}\n");

    fclose(output_);
    
    if (is_inspected_) {
      rename(tmp_path, graph_path);
    }
    else {
      remove(tmp_path);
    }
  }

  bool runOnFunction(Function& F) override {
    if (F.getParent()->getSourceFileName().find("INSPECTED") == std::string::npos) {
      return true;
    }
    
    is_inspected_ = true;
    if (F.getName()[0] == '_') {
      return true;
    }

    ProcessStaticData(F);
    InjectLogCalls(F);

    return true;
  }

 private:
  void InjectLogCalls(Function& F) {
    // Prepare builder for IR modification
    LLVMContext& Ctx = F.getContext();
    IRBuilder<> builder(Ctx);
    Type* retType = Type::getVoidTy(Ctx);

    // Prepare _LogFuncEntry function
    std::vector<Type*> logFuncEntryTypes = {
        builder.getInt8Ty()->getPointerTo()};
    FunctionType* logFuncEntryFuncType =
        FunctionType::get(retType, logFuncEntryTypes, false);
    FunctionCallee logFuncEntryFunc = F.getParent()->getOrInsertFunction(
        "_LogFuncEntry", logFuncEntryFuncType);

    // Insert a call to _LogFuncEntry function in the function begin
    BasicBlock& entryBB = F.getEntryBlock();
    builder.SetInsertPoint(&entryBB.front());
    Value* funcName = builder.CreateGlobalStringPtr(F.getName());
    Value* args[] = {funcName};
    builder.CreateCall(logFuncEntryFunc, args);

    // Prepare _LogCall function
    std::vector<Type*> logCallTypes = {builder.getInt8Ty()->getPointerTo(),
                                       builder.getInt8Ty()->getPointerTo(),
                                       Type::getInt64Ty(Ctx)};
    FunctionType* logCallFuncType =
        FunctionType::get(retType, logCallTypes, false);
    FunctionCallee logCallFunc =
        F.getParent()->getOrInsertFunction("_LogCall", logCallFuncType);

    // Prepare _LogRet function
    std::vector<Type*> logRetTypes = {Type::getInt64Ty(Ctx)};
    FunctionType* logRetFuncType =
        FunctionType::get(retType, logRetTypes, false);
    FunctionCallee logRetFunc =
        F.getParent()->getOrInsertFunction("_LogRet", logRetFuncType);

    for (auto& B : F) {
      for (auto& I : B) {
        if (auto* call = dyn_cast<CallInst>(&I)) {
          // Insert before call
          builder.SetInsertPoint(call);

          // Insert a call to callLogger function
          Function* callee = call->getCalledFunction();
          if (callee && (callee->getName()[0] != '_')) {
            Value* calleeName =
                builder.CreateGlobalStringPtr(callee->getName());
            Value* funcName = builder.CreateGlobalStringPtr(F.getName());

            Value* funcArgument = *call->value_op_begin();

            Value* args[] = {funcName, calleeName, funcArgument};
            builder.CreateCall(logCallFunc, args);
          }
        }

        if (auto* ret = dyn_cast<ReturnInst>(&I)) {
          if (F.getName() != "main") {
            // Insert before ret
            builder.SetInsertPoint(ret);

            // Insert a call to funcEndLogFunc function
            Value* funcArgument = *ret->value_op_begin();
            Value* args[] = {funcArgument};
            builder.CreateCall(logRetFunc, args);
          }
        }
      }
    }
  }

  void ProcessStaticData(Function& Func) {
    PrintNTabs();
    fprintf(output_, "subgraph cluster_%s {\n", Func.getName());

    depth_++;
    PrintNTabs();
    fprintf(output_, "label = \"%s\"\n", Func.getName());

    PrintNTabs();
    fprintf(output_, "%s[style = invis]\n", Func.getName());

    Instruction* prev_instruction = nullptr;
    for (auto& B : Func) {
      for (auto& I : B) {
        PrintNTabs();
        fprintf(output_, "%lu[label = \"%s\"];\n", &I, I.getOpcodeName());

        depth_ += 2;

        for (auto user : I.users()) {
          PrintNTabs();
          fprintf(output_, "%lu -> %lu [color = red]\n", &I, user);
        }

        if (prev_instruction) {
          PrintNTabs();
          fprintf(output_, "%lu -> %lu [color = yellow]\n", prev_instruction,
                  &I);
        }

        depth_ -= 2;
        prev_instruction = &I;
      }
    }

    depth_--;
    PrintNTabs();
    fprintf(output_, "}\n");
  }

  void PrintNTabs() {
    uint64_t depth = depth_;
    while (depth--) {
      fputc('\t', output_);
    }
  }

  FILE* output_;
  uint64_t depth_;
  bool is_inspected_;
};
}  // namespace

// Registering your pass in the LLVM
char MyPass::ID = 0;
static RegisterPass<MyPass> X("mypass", "My custom pass", false, true);
