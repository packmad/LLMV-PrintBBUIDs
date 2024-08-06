#include "PrintBBUIDs.h"

#include <random>

#include "llvm/IR/IRBuilder.h"
#include "llvm/Passes/PassPlugin.h"
#include "llvm/Passes/PassBuilder.h"

#include "llvm/IR/BasicBlock.h"
#include "llvm/IR/IRBuilder.h"
#include "llvm/Support/raw_ostream.h"

using namespace llvm;

#define DEBUG_TYPE "print-bb-uids"

//-----------------------------------------------------------------------------
// PrintBBUIDs implementation
//-----------------------------------------------------------------------------
bool PrintBBUIDs::runOnModule(Module &M) {
  bool InsertedAtLeastOnePrintf = false;

  auto &CTX = M.getContext();
  PointerType *PrintfArgTy = PointerType::getUnqual(Type::getInt8Ty(CTX));

  // STEP 1: Inject the declaration of printf
  // ----------------------------------------
  // Create (or _get_ in cases where it's already available) the following
  // declaration in the IR module:
  //    declare i32 @printf(i8*, ...)
  // It corresponds to the following C declaration:
  //    int printf(char *, ...)
  FunctionType *PrintfTy = FunctionType::get(
      IntegerType::getInt32Ty(CTX),
      PrintfArgTy,
      /*IsVarArgs=*/true);

  FunctionCallee Printf = M.getOrInsertFunction("printf", PrintfTy);

  // Set attributes as per inferLibFuncAttributes in BuildLibCalls.cpp
  Function *PrintfF = dyn_cast<Function>(Printf.getCallee());
  PrintfF->setDoesNotThrow();
  PrintfF->addParamAttr(0, Attribute::NoCapture);
  PrintfF->addParamAttr(0, Attribute::ReadOnly);


  // STEP 2: Inject a global variable that will hold the printf format string
  // ------------------------------------------------------------------------
  llvm::Constant *PrintfFormatStr = llvm::ConstantDataArray::getString(
      CTX, "~%d~\n");

  Constant *PrintfFormatStrVar =
      M.getOrInsertGlobal("PrintfFormatStr", PrintfFormatStr->getType());
  dyn_cast<GlobalVariable>(PrintfFormatStrVar)->setInitializer(PrintfFormatStr);

  // STEP 3: For each function in the module, inject a call to printf
  // ----------------------------------------------------------------
  int i = 0;
  for (Function &F : M) {
    if (F.isDeclaration())
      continue;

    for (BasicBlock &BB : F) {
      IRBuilder<> Builder(&*BB.getFirstInsertionPt());
      llvm::Value *FormatStrPtr =
              Builder.CreatePointerCast(PrintfFormatStrVar, PrintfArgTy, "formatStr");
       Builder.CreateCall(
        Printf, {FormatStrPtr, Builder.getInt32(i)});
      i++;

      InsertedAtLeastOnePrintf = true;
    }

    // Get an IR builder. Sets the insertion point to the top of the function
    //IRBuilder<> Builder(&*F.getEntryBlock().getFirstInsertionPt());

    // Inject a global variable that contains the function name
    //auto FuncName = Builder.CreateGlobalStringPtr(F.getName());

    // Printf requires i8*, but PrintfFormatStrVar is an array: [n x i8]. Add
    // a cast: [n x i8] -> i8*
    

    // The following is visible only if you pass -debug on the command line
    // *and* you have an assert build.
    LLVM_DEBUG(dbgs() << " Injecting call to printf inside " << F.getName()
                      << "\n");

    // Finally, inject a call to printf
   
  }

  return InsertedAtLeastOnePrintf;
}

PreservedAnalyses PrintBBUIDs::run(llvm::Module &M,
                                       llvm::ModuleAnalysisManager &) {
  bool Changed =  runOnModule(M);

  return (Changed ? llvm::PreservedAnalyses::none()
                  : llvm::PreservedAnalyses::all());
}


//-----------------------------------------------------------------------------
// New PM Registration
//-----------------------------------------------------------------------------
llvm::PassPluginLibraryInfo getPrintBBUIDsPluginInfo() {
  return {LLVM_PLUGIN_API_VERSION, "print-bb-uids", LLVM_VERSION_STRING,
          [](PassBuilder &PB) {
            PB.registerPipelineParsingCallback(
                [](StringRef Name, ModulePassManager &MPM,
                   ArrayRef<PassBuilder::PipelineElement>) {
                  if (Name == "print-bb-uids") {
                    MPM.addPass(PrintBBUIDs());
                    return true;
                  }
                  return false;
                });
          }};
}

extern "C" LLVM_ATTRIBUTE_WEAK ::llvm::PassPluginLibraryInfo
llvmGetPassPluginInfo() {
  return getPrintBBUIDsPluginInfo();
}
