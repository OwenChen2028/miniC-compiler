#include <llvm-c/Core.h>
#include <llvm-c/IRReader.h>
#include <llvm-c/Types.h>
#include <stdio.h>
#include <stdlib.h>
#include <unordered_map>
#include <unordered_set>
#include <vector>

LLVMModuleRef createLLVMModule(char *filename) {
  char *err = 0;

  LLVMMemoryBufferRef ll_f = 0;
  LLVMModuleRef m = 0;

  LLVMCreateMemoryBufferWithContentsOfFile(filename, &ll_f, &err);

  if (err != NULL) {
    fprintf(stderr, "%s\n", err);
    return NULL;
  }

  LLVMParseIRInContext(LLVMGetGlobalContext(), ll_f, &m, &err);

  if (err != NULL) {
    fprintf(stderr, "%s\n", err);
  }

  return m;
}

struct subexpr {
  LLVMOpcode opcode;
  std::vector<LLVMValueRef> operands;

  subexpr(LLVMOpcode op, std::vector<LLVMValueRef> ops)
      : opcode(op), operands(std::move(ops)) {}

  bool operator==(const subexpr &other) const {
    return opcode == other.opcode && operands == other.operands;
  }
};

struct subexprHash {
  size_t operator()(const subexpr &s) const {
    size_t hash = s.opcode;
    for (LLVMValueRef ref : s.operands)
      hash += (size_t)ref;
    return hash;
  }
};

int doCommonSubexprElim(LLVMBasicBlockRef bb) {
  int changed = 0;
  std::unordered_map<subexpr, LLVMValueRef, subexprHash> visited;

  for (LLVMValueRef instruction = LLVMGetFirstInstruction(bb); instruction;
       instruction = LLVMGetNextInstruction(instruction)) {

    if (LLVMIsACallInst(instruction) || LLVMIsAStoreInst(instruction) ||
        LLVMIsATerminatorInst(instruction) || LLVMIsAAllocaInst(instruction)) {
      continue;
    }

    LLVMOpcode op = LLVMGetInstructionOpcode(instruction);

    std::vector<LLVMValueRef> opers;
    for (int i = 0; i < LLVMGetNumOperands(instruction); ++i)
      opers.push_back(LLVMGetOperand(instruction, i));

    subexpr curr(op, opers);
    auto prev = visited.find(curr);

    if (prev != visited.end()) { // found curr in visited
      if (!LLVMIsALoadInst(instruction)) {
        changed = 1;
        LLVMReplaceAllUsesWith(instruction, prev->second);
      } else {
        int safe = 1;
        for (LLVMValueRef instr = LLVMGetNextInstruction(prev->second); instr;
             instr = LLVMGetNextInstruction(instr)) {
          if (instr == instruction)
            break;
          if (LLVMIsAStoreInst(instr)) {
            if (LLVMGetOperand(instr, 1) == LLVMGetOperand(instruction, 0)) {
              safe = 0;
              break;
            }
          }
        }
        if (safe) {
          changed = 1;
          LLVMReplaceAllUsesWith(instruction, prev->second);
        } else
          prev->second = instruction; // replace old inst, it has a store after
      }
    } else
      visited.emplace(std::move(curr), instruction);
  }

  return 1;
}

int doDeadCodeElim(LLVMBasicBlockRef bb) {
  int changed = 0;

  for (LLVMValueRef instruction = LLVMGetFirstInstruction(bb); instruction;) {
    LLVMValueRef nextInstr = LLVMGetNextInstruction(instruction);

    if (LLVMIsACallInst(instruction) || LLVMIsAStoreInst(instruction) ||
        LLVMIsATerminatorInst(instruction) || LLVMIsAAllocaInst(instruction)) {
      instruction = nextInstr;
      continue; // inst has side effects, do not elim
    }

    LLVMUseRef firstUse = LLVMGetFirstUse(instruction);
    if (firstUse == NULL) {
      LLVMInstructionEraseFromParent(instruction);
      changed = 1;
    }

    instruction = nextInstr;
  }

  return changed;
}

int doConstantFolding(LLVMBasicBlockRef bb) {
  int changed = 0;

  for (LLVMValueRef instruction = LLVMGetFirstInstruction(bb); instruction;
       instruction = LLVMGetNextInstruction(instruction)) {

    LLVMOpcode op = LLVMGetInstructionOpcode(instruction);

    if (op == LLVMAdd || op == LLVMSub || op == LLVMMul) {
      LLVMValueRef operA = LLVMGetOperand(instruction, 0);
      LLVMValueRef operB = LLVMGetOperand(instruction, 1);

      if (LLVMIsConstant(operA) && LLVMIsConstant(operB)) {
        LLVMValueRef constInstr;

        switch (op) {
        case LLVMAdd:
          constInstr = LLVMConstAdd(operA, operB);
          break;
        case LLVMSub:
          constInstr = LLVMConstSub(operA, operB);
          break;
        case LLVMMul:
          constInstr = LLVMConstMul(operA, operB);
          break;
        }

        changed = 1;
        LLVMReplaceAllUsesWith(instruction, constInstr);
      }
    }
  }

  return changed;
}

int doConstantPropogation(LLVMModuleRef module) {
  std::unordered_map<LLVMBasicBlockRef, std::unordered_set<LLVMValueRef>> gen;

  for (LLVMValueRef function = LLVMGetFirstFunction(module); function;
       function = LLVMGetNextFunction(function)) {
    for (LLVMBasicBlockRef basicBlock = LLVMGetFirstBasicBlock(function);
         basicBlock; basicBlock = LLVMGetNextBasicBlock(basicBlock)) {
      for (LLVMValueRef instruction = LLVMGetFirstInstruction(basicBlock);
           instruction; instruction = LLVMGetNextInstruction(instruction)) {

        if (!LLVMIsAStoreInst(instruction))
          continue;

        for (auto iter = gen[basicBlock].begin();
             iter != gen[basicBlock].end();) {
          if (LLVMGetOperand(*iter, 1) ==
              LLVMGetOperand(instruction, 1)) // another store at same location
            iter = gen[basicBlock].erase(iter); // remove it and get next
          else
            ++iter;
        }

        gen[basicBlock].insert(instruction);
      }
    }
  }

  std::unordered_map<LLVMBasicBlockRef, std::unordered_set<LLVMValueRef>> kill;

  for (LLVMValueRef function = LLVMGetFirstFunction(module); function;
       function = LLVMGetNextFunction(function)) {

    std::unordered_set<LLVMValueRef> stores;

    for (LLVMBasicBlockRef basicBlock = LLVMGetFirstBasicBlock(function);
         basicBlock; basicBlock = LLVMGetNextBasicBlock(basicBlock)) {
      for (LLVMValueRef instruction = LLVMGetFirstInstruction(basicBlock);
           instruction; instruction = LLVMGetNextInstruction(instruction)) {

        if (LLVMIsAStoreInst(instruction))
          stores.insert(instruction);
      }
    }

    for (LLVMBasicBlockRef basicBlock = LLVMGetFirstBasicBlock(function);
         basicBlock; basicBlock = LLVMGetNextBasicBlock(basicBlock)) {
      for (LLVMValueRef instruction = LLVMGetFirstInstruction(basicBlock);
           instruction; instruction = LLVMGetNextInstruction(instruction)) {

        if (!LLVMIsAStoreInst(instruction))
          continue;

        for (LLVMValueRef s : stores) {
          if (s == instruction)
            continue;
          if (LLVMGetOperand(s, 1) == LLVMGetOperand(instruction, 1))
            kill[basicBlock].insert(s);
        }
      }
    }

    std::unordered_set<LLVMValueRef> in;
    std::unordered_set<LLVMValueRef> out;

    
  }
}

void doOptimizations(LLVMModuleRef module) {
  for (LLVMValueRef function = LLVMGetFirstFunction(module); function;
       function = LLVMGetNextFunction(function)) {
    for (LLVMBasicBlockRef basicBlock = LLVMGetFirstBasicBlock(function);
         basicBlock; basicBlock = LLVMGetNextBasicBlock(basicBlock)) {

      doConstantFolding(basicBlock);
      doCommonSubexprElim(basicBlock);
      while (doDeadCodeElim(basicBlock))
        ; // repeat until stable
    }
  }
}

int main(int argc, char **argv) {
  LLVMModuleRef m;

  if (argc == 2) {
    m = createLLVMModule(argv[1]);
  } else {
    fprintf(stderr, "No file specified.\n");
    return 1;
  }

  if (m != NULL) {
    doOptimizations(m);
    LLVMDumpModule(m);
  } else {
    fprintf(stderr, "Module reference is null.\n");
    return 1;
  }
}
