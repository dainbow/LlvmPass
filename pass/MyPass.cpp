/*
 * The code for your pass goes here.
 */

// Some header files from LLVM
#include "llvm/Pass.h"
#include "llvm/IR/Function.h"
#include "llvm/Support/raw_ostream.h"

using namespace llvm;

namespace {

    // Define your Pass as a subclass of one of the existing pass interfaces.
    struct MyPass : public FunctionPass {
        static char ID;
        MyPass() : FunctionPass(ID) {}

        // This function is provided by FunctionPass. It runs on every function
        // in a LLVM IR module and passes a reference to the current function
        // as parameter
        bool runOnFunction(Function &Func) override {
            outs() << "Running on function " << Func.getName() << "\n";
            
            return true;
        }
    };
}

// Registering your pass in the LLVM
char MyPass::ID = 0;
static RegisterPass<MyPass> X ("mypass", "My custom pass", false, true);
