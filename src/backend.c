#include "backend.h"
#include <algorithm>
#include <cstdio>
#include <llvm-c/Core.h>
#include <string>
#include <unordered_map>
#include <unordered_set>
#include <vector>

std::unordered_map<LLVMValueRef, Register> reg_map;

std::unordered_map<LLVMValueRef, int> inst_index;
std::unordered_map<LLVMValueRef, std::pair<int, int>> live_range;

std::vector<LLVMValueRef> sorted_list;

FILE *out_fp;

int compare_instr(LLVMValueRef instrA, LLVMValueRef instrB) {
  return live_range[instrA].second > live_range[instrB].second;
} // decreasing order

void compute_liveness(LLVMBasicBlockRef bb) {
  inst_index.clear();
  live_range.clear();
  sorted_list.clear();

  int idx = 0;
  for (LLVMValueRef instruction = LLVMGetFirstInstruction(bb); instruction;
       instruction = LLVMGetNextInstruction(instruction)) {

    if (LLVMIsAAllocaInst(instruction))
      continue;

    inst_index[instruction] = idx;
    live_range[instruction] = std::make_pair(idx, idx);
    sorted_list.push_back(instruction);

    ++idx;
  }

  for (LLVMValueRef instruction = LLVMGetFirstInstruction(bb); instruction;
       instruction = LLVMGetNextInstruction(instruction)) {

    if (LLVMIsAAllocaInst(instruction))
      continue;

    for (int i = 0; i < LLVMGetNumOperands(instruction); ++i) {
      LLVMValueRef oper = LLVMGetOperand(instruction, i);
      auto first = inst_index.find(oper);
      if (first != inst_index.end())
        live_range[oper] = std::make_pair(first->second, inst_index[instruction]);
    }
  }

  std::sort(sorted_list.begin(), sorted_list.end(), compare_instr);
}

LLVMValueRef find_spill(LLVMValueRef instr) {
  for (LLVMValueRef v : sorted_list) {
    if (live_range[v].first <= live_range[instr].second &&
        live_range[instr].first <= live_range[v].second) {
      if (reg_map.count(v) && reg_map[v] != nullreg)
        return v;
    }
  }
  return NULL;
}

void alloc_registers(LLVMValueRef function) {
  for (LLVMBasicBlockRef basicBlock = LLVMGetFirstBasicBlock(function);
       basicBlock; basicBlock = LLVMGetNextBasicBlock(basicBlock)) {

    std::unordered_set<Register> available = {ebx, ecx, edx};
    compute_liveness(basicBlock);

    for (LLVMValueRef instr = LLVMGetFirstInstruction(basicBlock); instr;
         instr = LLVMGetNextInstruction(instr)) {

      if (LLVMIsAAllocaInst(instr))
        continue;

      if ((LLVMIsACallInst(instr) &&
           LLVMGetTypeKind(LLVMTypeOf(instr)) == LLVMVoidTypeKind) ||
          LLVMIsAStoreInst(instr) || LLVMIsATerminatorInst(instr)) {

        for (int i = 0; i < LLVMGetNumOperands(instr); ++i) {
          LLVMValueRef oper = LLVMGetOperand(instr, i);
          if (live_range.count(oper) &&
              live_range[oper].second == inst_index[instr]) {
            auto reg = reg_map.find(oper);
            if (reg != reg_map.end() && reg->second != nullreg)
              available.insert(reg->second);
          }
        }
      } else {
        LLVMOpcode op = LLVMGetInstructionOpcode(instr);
        if (op == LLVMAdd || op == LLVMSub || op == LLVMMul) {
          LLVMValueRef operA = LLVMGetOperand(instr, 0);
          if (live_range.count(operA) &&
              live_range[operA].second == inst_index[instr]) {
            auto regA = reg_map.find(operA);
            if (regA != reg_map.end() && regA->second != nullreg) {
              reg_map[instr] = regA->second;

              LLVMValueRef operB = LLVMGetOperand(instr, 1);
              if (live_range.count(operB) &&
                  live_range[operB].second == inst_index[instr]) {
                auto regB = reg_map.find(operB);
                if (regB != reg_map.end() && regB->second != nullreg)
                  available.insert(regB->second);
              } // only if instr has been assigned a register
            }
          } // if can't reuse 1st oper, fall through to next check
        }

        if (reg_map.find(instr) == reg_map.end()) {
          if (!available.empty()) {
            auto reg = available.begin();
            reg_map[instr] = *reg;
            available.erase(reg);
          } else {
            LLVMValueRef v = find_spill(instr);
            if (v != NULL) {
              if (compare_instr(instr, v)) {
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
            if (live_range.count(oper) &&
                live_range[oper].second == inst_index[instr]) {
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

std::unordered_map<LLVMBasicBlockRef, std::string> label_map;

void createBBLabels(LLVMValueRef function) {
  label_map.clear();
  int counter = 0;
  for (LLVMBasicBlockRef basicBlock = LLVMGetFirstBasicBlock(function);
       basicBlock; basicBlock = LLVMGetNextBasicBlock(basicBlock)) {
    label_map[basicBlock] = ".L" + std::to_string(counter);
    counter++;
  }
}

void printDirectives(LLVMValueRef function) {
  const char *name = LLVMGetValueName(function);
  fprintf(out_fp, ".text\n");
  fprintf(out_fp, ".globl %s\n", name);
  fprintf(out_fp, ".type %s, @function\n", name);
  fprintf(out_fp, "%s:\n", name);
}

void printFunctionEnd() {
  fprintf(out_fp, "\tleave\n");
  fprintf(out_fp, "\tret\n");
}

int localMem;
std::unordered_map<LLVMValueRef, int> offset_map;

void getOffsetMap(LLVMValueRef function) {
  localMem = 4;
  offset_map.clear();

  LLVMValueRef param = NULL;
  if (LLVMCountParams(function) > 0) {
    param = LLVMGetParam(function, 0);
    offset_map[param] = 8;
  }

  for (LLVMBasicBlockRef basicBlock = LLVMGetFirstBasicBlock(function);
       basicBlock; basicBlock = LLVMGetNextBasicBlock(basicBlock)) {
    for (LLVMValueRef instr = LLVMGetFirstInstruction(basicBlock); instr;
       instr = LLVMGetNextInstruction(instr)) {

      if (LLVMIsAAllocaInst(instr)) {
        localMem += 4;
        offset_map[instr] = -localMem;
      } else if (LLVMIsAStoreInst(instr)) {
        LLVMValueRef operA = LLVMGetOperand(instr, 0);
        if (operA == param) { // only on the param's initial store
          int x = offset_map[operA];
          offset_map[LLVMGetOperand(instr, 1)] = x;
        } else if (!LLVMIsAConstant(operA)) {
          int x = offset_map[LLVMGetOperand(instr, 1)];
          offset_map[operA] = x;
        }
      } else if (LLVMIsALoadInst(instr)) {
        int x = offset_map[LLVMGetOperand(instr, 0)];
        offset_map[instr] = x;
      }
    }
  }
}

const char *reg_to_str(Register reg) {
  switch (reg) {
  case ebx:
    return "ebx";
  case ecx:
    return "ecx";
  case edx:
    return "edx";
  default:
    return "null";
  }
}

void generate_code(LLVMModuleRef module) {
  for (LLVMValueRef function = LLVMGetFirstFunction(module); function;
       function = LLVMGetNextFunction(function)) {

    if (LLVMGetFirstBasicBlock(function) == NULL)
      continue;

    alloc_registers(function);

    createBBLabels(function);
    printDirectives(function);
    getOffsetMap(function);

    fprintf(out_fp, "pushl %%ebp\n");
    fprintf(out_fp, "movl %%esp, %%ebp\n");
    fprintf(out_fp, "subl %d, %%esp\n", localMem);
    fprintf(out_fp, "pushl %%ebx\n");

    for (LLVMBasicBlockRef basicBlock = LLVMGetFirstBasicBlock(function);
         basicBlock; basicBlock = LLVMGetNextBasicBlock(basicBlock)) {

      fprintf(out_fp, "%s:\n", label_map[basicBlock].c_str());

      for (LLVMValueRef instr = LLVMGetFirstInstruction(basicBlock); instr;
           instr = LLVMGetNextInstruction(instr)) {

        LLVMOpcode opcode = LLVMGetInstructionOpcode(instr);
        switch (opcode) {
        case LLVMRet: {
          LLVMValueRef operA = LLVMGetOperand(instr, 0);
          if (LLVMIsAConstant(operA)) {
            int valA = (int)LLVMConstIntGetSExtValue(operA);
            fprintf(out_fp, "movl $%d, %%eax\n", valA);
          } else {
            auto reg = reg_map.find(operA);
            if (reg != reg_map.end()) {
              if (reg->second == nullreg) {
                fprintf(out_fp, "movl %d(%%ebp), %%eax\n", offset_map[operA]);
              } else {
                fprintf(out_fp, "movl %%%s, %%eax\n", reg_to_str(reg->second));
              }
            }
          }
          fprintf(out_fp, "popl %%ebx\n");
          printFunctionEnd();
          break;
        }

        case LLVMLoad: {
          auto reg = reg_map.find(instr);
          if (reg != reg_map.end() && reg->second != nullreg) {
            int c = offset_map[LLVMGetOperand(instr, 0)];
            fprintf(out_fp, "movl %d(%%ebp), %%%s\n", c, reg_to_str(reg->second));
          }
          break;
        }

        case LLVMStore: {
          LLVMValueRef operA = LLVMGetOperand(instr, 0);
          if (LLVMCountParams(function) > 0 && operA == LLVMGetParam(function, 0))
            break;
          if (LLVMIsAConstant(operA)) {
            int c = offset_map[LLVMGetOperand(instr, 1)];
            int valA = (int)LLVMConstIntGetSExtValue(operA);
            fprintf(out_fp, "movl $%d, %d(%%ebp)\n", valA, c);
          } else {
            auto reg = reg_map.find(operA);
            if (reg != reg_map.end() && reg->second != nullreg) {
              int c = offset_map[LLVMGetOperand(instr, 1)];
              fprintf(out_fp, "movl %%%s, %d(%%ebp)\n", reg_to_str(reg->second), c);
            } else {
              int c1 = offset_map[operA];
              fprintf(out_fp, "movl %d(%%ebp), %%eax\n", c1);
              int c2 = offset_map[LLVMGetOperand(instr, 1)];
              fprintf(out_fp, "movl %%eax, %d(%%ebp)\n", c2);
            }
          }
          break;
        }

        case LLVMCall: {
          fprintf(out_fp, "pushl %%ecx\n");
          fprintf(out_fp, "pushl %%edx\n");

          LLVMValueRef func = LLVMGetCalledValue(instr);

          if (LLVMCountParams(func) > 0) {
            LLVMValueRef p = LLVMGetParam(func, 1);
            if (LLVMIsAConstant(p)) {
              int valP = (int)LLVMConstIntGetSExtValue(p);
              fprintf(out_fp, "pushl $%d\n", valP);
            }
            else {
              auto reg = reg_map.find(p);
              if (reg != reg_map.end() && reg->second != nullreg)
                fprintf(out_fp, "pushl %%%s\n", reg_to_str(reg->second));
              else
                fprintf(out_fp, "pushl %d(%%ebp)\n", offset_map[p]);
            }
          }

          fprintf(out_fp, "call %s\n", LLVMGetValueName(func));

          if (LLVMCountParams(func) > 0)
            fprintf(out_fp, "addl $4, %%esp\n");

          fprintf(out_fp, "popl %%edx\n");
          fprintf(out_fp, "popl %%ecx\n");

          if (LLVMGetTypeKind(LLVMTypeOf(instr)) == LLVMVoidTypeKind) {
            auto reg = reg_map.find(instr);
            if (reg != reg_map.end() && reg->second != nullreg)
              fprintf(out_fp, "movl %%eax, %%%s\n", reg_to_str(reg->second));
            else
              fprintf(out_fp, "movl %%eax, %d(%%ebp)\n", offset_map[instr]);
          }
          break;
        }

        case LLVMBr:
          break;

        case LLVMAdd:
        case LLVMSub:
        case LLVMMul:
          break;

        case LLVMICmp:
          break;
        }
      }
    }
  }
}
