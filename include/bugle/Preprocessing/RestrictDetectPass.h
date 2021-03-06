#ifndef BUGLE_PREPROCESSING_RESTRICTDETECTPASS_H
#define BUGLE_PREPROCESSING_RESTRICTDETECTPASS_H

#include "bugle/Translator/TranslateModule.h"
#include "llvm/Pass.h"
#include "llvm/IR/DebugInfo.h"

namespace bugle {

class RestrictDetectPass : public llvm::FunctionPass {
private:
  llvm::Module *M;
  llvm::DebugInfoFinder DIF;
  TranslateModule::SourceLanguage SL;
  std::set<std::string> GPUEntryPoints;
  TranslateModule::AddressSpaceMap AddressSpaces;

  const llvm::DISubprogram *getDebugInfo(llvm::Function *F);
  std::string getFunctionLocation(llvm::Function *F);
  bool ignoreArgument(unsigned i, const llvm::DISubprogram *DIS);
  void doRestrictCheck(llvm::Function &F);

public:
  static char ID;

  RestrictDetectPass(TranslateModule::SourceLanguage SL,
                     std::set<std::string> &EP,
                     TranslateModule::AddressSpaceMap &AS)
      : FunctionPass(ID), M(0), SL(SL), GPUEntryPoints(EP), AddressSpaces(AS) {}

  llvm::StringRef getPassName() const override {
    return "Detect restrict usage on global pointers";
  }

  void getAnalysisUsage(llvm::AnalysisUsage &AU) const override {
    AU.setPreservesAll();
  }

  bool doInitialization(llvm::Module &M) override;
  bool runOnFunction(llvm::Function &F) override;
};
}

#endif
