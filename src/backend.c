#include "backend.h"
#include <llvm-c/Core.h>
#include <unordered_map>
#include <unordered_set>

std::unordered_map<LLVMValueRef, Register> reg_map;

std::unordered_map<LLVMValueRef, int> inst_index;
std::unordered_map<LLVMValueRef, std::pair<int, int>> live_range;

void compute_liveness(LLVMBasicBlockRef bb) {
  inst_index.clear();
  live_range.clear();

  int idx = 0;
  for (LLVMValueRef instruction = LLVMGetFirstInstruction(bb); instruction;
       instruction = LLVMGetNextInstruction(instruction)) {

    if (LLVMIsAAllocaInst(instruction))
      continue;

    inst_index[instruction] = idx;
    live_range[instruction] = std::make_pair(idx, idx);

    ++idx;
  }

  for (LLVMValueRef instruction = LLVMGetFirstInstruction(bb); instruction;
       instruction = LLVMGetNextInstruction(instruction)) {

    if (LLVMIsAAllocaInst(instruction))
      continue;

    for (int i = 0; i < LLVMGetNumOperands(instruction); ++i) {
      LLVMValueRef oper = LLVMGetOperand(instruction, i);
      live_range[oper] = std::make_pair(inst_index[oper], inst_index[instruction]);
    }
  }
}

int compare_instr(LLVMValueRef instrA, LLVMValueRef instrB) {
  return live_range[instrA].second < live_range[instrB].second;
}

LLVMValueRef find_spill(LLVMValueRef instr) {}

void alloc_registers(LLVMModuleRef module) {
  for (LLVMValueRef function = LLVMGetFirstFunction(module); function;
       function = LLVMGetNextFunction(function)) {
    for (LLVMBasicBlockRef basicBlock = LLVMGetFirstBasicBlock(function);
         basicBlock; basicBlock = LLVMGetNextBasicBlock(basicBlock)) {

      std::unordered_set<Register> available = {ebx, ecx, edx};
      compute_liveness(basicBlock);

      for (LLVMValueRef instr = LLVMGetFirstInstruction(basicBlock); instr;
           instr = LLVMGetNextInstruction(instr)) {

        if (LLVMIsAAllocaInst(instr))
          continue;

        if (LLVMIsACallInst(instr) || LLVMIsAStoreInst(instr) ||
            LLVMIsATerminatorInst(instr)) {

          for (int i = 0; i < LLVMGetNumOperands(instr); ++i) {
            LLVMValueRef oper = LLVMGetOperand(instr, i);
            if (live_range[oper].second == inst_index[instr]) {
              auto reg = reg_map.find(oper);
              if (reg != reg_map.end() && reg->second != nullreg)
                available.insert(reg->second);
            }
          }
        } else {
          LLVMOpcode op = LLVMGetInstructionOpcode(instr);
          if (op == LLVMAdd || op == LLVMSub || op == LLVMMul) {
            LLVMValueRef operA = LLVMGetOperand(instr, 0);
            if (live_range[operA].second == inst_index[instr]) {
              auto regA = reg_map.find(operA);
              if (regA != reg_map.end() && regA->second != nullreg) {
                reg_map[instr] = regA->second;

                LLVMValueRef operB = LLVMGetOperand(instr, 1);
                if (live_range[operB].second == inst_index[instr]) {
                  auto regB = reg_map.find(operB);
                  if (regB != reg_map.end() && regB->second != nullreg)
                    available.insert(regB->second);
                } // only if instr has been assigned a register
              }
            }
          }

          if (reg_map.find(instr) == reg_map.end()) { // still not assigned
            if (!available.empty()) {
              auto reg = available.begin();
              reg_map[instr] = *reg;
              available.erase(reg);
            } else {
              LLVMValueRef v = find_spill(instr);
              if (v != NULL) {
                if (compare_instr(v, instr)) {
                  reg_map[instr] = nullreg;
                } else {
                  reg_map[instr] = reg_map[v];
                  reg_map[v] = nullreg;
                }
              } else {
                reg_map[instr] = nullreg;
              }
            }

            for (int i = 0; i < LLVMGetNumOperands(instr); ++i) {
              LLVMValueRef oper = LLVMGetOperand(instr, i);
              if (live_range[oper].second == inst_index[instr]) {
                auto reg = reg_map.find(oper);
                if (reg != reg_map.end() && reg->second != nullreg)
                  available.insert(reg->second);
              }
            }
          }
        }
      }
    }
  }
}
