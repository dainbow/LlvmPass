/*
 * The code for your pass goes here.
 */

// Some header files from LLVM
#include "llvm/Pass.h"
#include "llvm/IR/Function.h"
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
            fprintf(output_, "rankdir=HR; outputOrder=nodesfirst;\n");
        }

        ~MyPass() {
            depth_--;
            PrintNTabs();
            fprintf(output_, "}\n");
            
            fclose(output_);
        }

        // This function is provided by FunctionPass. It runs on every function
        // in a LLVM IR module and passes a reference to the current function
        // as parameter
        bool runOnFunction(Function &Func) override {
            PrintNTabs();
            fprintf(output_, "subgraph %s {\n", Func.getName());
            
            PrintNTabs();
            fprintf(output_, "label = \"%s\"\n", Func.getName());
            depth_++;

            for (auto& B : Func) {
                for (auto& I : B) {
                    PrintNTabs();
                    fprintf(output_, "\t%lu[label = \"%s\"];\n", &I, I.getOpcodeName());
                    depth_++;

                    for (auto* user : I.users()) {
                        PrintNTabs();
                        fprintf(output_, "\t\t%lu -> %lu\n", &I, user);
                    }

                    depth_--;
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
}

// Registering your pass in the LLVM
char MyPass::ID = 0;
static RegisterPass<MyPass> X ("mypass", "My custom pass", false, true);
