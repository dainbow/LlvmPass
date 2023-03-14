/*
 * The code for your pass goes here.
 */

// Some header files from LLVM
#include "llvm/IR/Function.h"
#include "llvm/Pass.h"
#include "llvm/Support/raw_ostream.h"

const char graph_path[] = "./graphs/static.txt";

using namespace llvm;

namespace {

// Define your Pass as a subclass of one of the existing pass interfaces.
struct MyPass : public FunctionPass {
  static char ID;
  MyPass() : FunctionPass(ID), output_(nullptr), depth_(0) {
    output_ = fopen(graph_path, "w");
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
  }

  bool runOnFunction(Function& Func) override {
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
          fprintf(output_, "%lu -> %lu [color = yellow]\n", prev_instruction, &I);
        }

        depth_ -= 2;
        prev_instruction = &I;
      }
    }

    depth_--;
    PrintNTabs();
    fprintf(output_, "}\n");

    return true;
  }

 private:
  void PrintNTabs() {
    uint64_t depth = depth_;
    while (depth--) {
      fputc('\t', output_);
    }
  }

  FILE* output_;
  uint64_t depth_;
};
}  // namespace

// Registering your pass in the LLVM
char MyPass::ID = 0;
static RegisterPass<MyPass> X("mypass", "My custom pass", false, true);
