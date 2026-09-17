#include "optimizer.hpp"
#include <cstdio>
#include <llvm-c/Analysis.h>
#include <llvm-c/Core.h>
#include <llvm-c/IRReader.h>

int main(int argc, char *argv[]) {
  if (argc != 3) {
    fprintf(stderr, "Usage: optimizer_runner <input.ll> <output.ll>\n");
    return 1;
  }

  LLVMContextRef context = LLVMContextCreate();
  LLVMMemoryBufferRef buffer = nullptr;
  LLVMModuleRef module = nullptr;
  char *message = nullptr;

  if (LLVMCreateMemoryBufferWithContentsOfFile(argv[1], &buffer, &message)) {
    fprintf(stderr, "%s\n", message);
    LLVMDisposeMessage(message);
    LLVMContextDispose(context);
    return 1;
  }

  if (LLVMParseIRInContext(context, buffer, &module, &message)) {
    fprintf(stderr, "%s\n", message);
    LLVMDisposeMessage(message);
    LLVMContextDispose(context);
    return 1;
  }

  optimize_ir(module);

  if (LLVMVerifyModule(module, LLVMReturnStatusAction, &message)) {
    fprintf(stderr, "%s\n", message);
    LLVMDisposeMessage(message);
    LLVMDisposeModule(module);
    LLVMContextDispose(context);
    return 1;
  }

  if (LLVMPrintModuleToFile(module, argv[2], &message)) {
    fprintf(stderr, "%s\n", message);
    LLVMDisposeMessage(message);
    LLVMDisposeModule(module);
    LLVMContextDispose(context);
    return 1;
  }

  LLVMDisposeModule(module);
  LLVMContextDispose(context);
  return 0;
}
