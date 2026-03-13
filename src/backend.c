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

bool compare_instr(LLVMValueRef instrA, LLVMValueRef instrB) {
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
  reg_map.clear();

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
              if (compare_instr(instr, v)) { // reversed bc dec order
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
          int x = offset_map.at(operA);
          offset_map[LLVMGetOperand(instr, 1)] = x;
        } else if (!LLVMIsAConstantInt(operA)) {
          int x = offset_map.at(LLVMGetOperand(instr, 1));
          offset_map[operA] = x;
        }
      } else if (LLVMIsALoadInst(instr)) {
        int x = offset_map.at(LLVMGetOperand(instr, 0));
        offset_map[instr] = x;
      }
    }
  }
}

const char *reg_to_str(Register reg) {
  switch (reg) {
  case eax:
    return "eax";
  case ebx:
    return "ebx";
  case ecx:
    return "ecx";
  case edx:
    return "edx";
  default:
    return NULL;
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

    fprintf(out_fp, "\tpushl %%ebp\n");
    fprintf(out_fp, "\tmovl %%esp, %%ebp\n");
    fprintf(out_fp, "\tsubl %d, %%esp\n", localMem);
    fprintf(out_fp, "\tpushl %%ebx\n");

    for (LLVMBasicBlockRef basicBlock = LLVMGetFirstBasicBlock(function);
         basicBlock; basicBlock = LLVMGetNextBasicBlock(basicBlock)) {

      fprintf(out_fp, "%s:\n", label_map.at(basicBlock).c_str());

      for (LLVMValueRef instr = LLVMGetFirstInstruction(basicBlock); instr;
           instr = LLVMGetNextInstruction(instr)) {

        LLVMOpcode opcode = LLVMGetInstructionOpcode(instr);
        switch (opcode) {
        case LLVMRet: {
          if (LLVMGetNumOperands(instr) > 0) {
            LLVMValueRef operA = LLVMGetOperand(instr, 0);
            if (LLVMIsAConstantInt(operA)) {
              int valA = (int)LLVMConstIntGetSExtValue(operA);
              fprintf(out_fp, "\tmovl $%d, %%eax\n", valA);
            } else {
              auto reg = reg_map.find(operA);
              if (reg != reg_map.end()) {
                if (reg->second == nullreg) {
                  fprintf(out_fp, "\tmovl %d(%%ebp), %%eax\n", offset_map.at(operA));
                } else {
                  fprintf(out_fp, "\tmovl %%%s, %%eax\n", reg_to_str(reg->second));
                }
              }
            }
          }
          fprintf(out_fp, "\tpopl %%ebx\n");
          printFunctionEnd();
          break;
        }

        case LLVMLoad: {
          auto reg = reg_map.find(instr);
          if (reg != reg_map.end() && reg->second != nullreg) {
            int c = offset_map.at(LLVMGetOperand(instr, 0));
            fprintf(out_fp, "\tmovl %d(%%ebp), %%%s\n", c, reg_to_str(reg->second));
          }
          break;
        }

        case LLVMStore: {
          LLVMValueRef operA = LLVMGetOperand(instr, 0);
          if (LLVMCountParams(function) > 0 &&
              operA == LLVMGetParam(function, 0))
            break;
          if (LLVMIsAConstantInt(operA)) {
            int c = offset_map.at(LLVMGetOperand(instr, 1));
            int valA = (int)LLVMConstIntGetSExtValue(operA);
            fprintf(out_fp, "\tmovl $%d, %d(%%ebp)\n", valA, c);
          } else {
            auto reg = reg_map.find(operA);
            if (reg != reg_map.end() && reg->second != nullreg) {
              int c = offset_map.at(LLVMGetOperand(instr, 1));
              fprintf(out_fp, "\tmovl %%%s, %d(%%ebp)\n", reg_to_str(reg->second), c);
            } else {
              int c1 = offset_map.at(operA);
              fprintf(out_fp, "\tmovl %d(%%ebp), %%eax\n", c1);
              int c2 = offset_map.at(LLVMGetOperand(instr, 1));
              fprintf(out_fp, "\tmovl %%eax, %d(%%ebp)\n", c2);
            }
          }
          break;
        }

        case LLVMCall: {
          fprintf(out_fp, "\tpushl %%ecx\n");
          fprintf(out_fp, "\tpushl %%edx\n");

          LLVMValueRef func = LLVMGetCalledValue(instr);

          if (LLVMGetNumOperands(instr) >= 2) { // func has param
            LLVMValueRef p = LLVMGetOperand(instr, 0);
            if (LLVMIsAConstantInt(p)) {
              int valP = (int)LLVMConstIntGetSExtValue(p);
              fprintf(out_fp, "\tpushl $%d\n", valP);
            } else {
              auto reg = reg_map.find(p);
              if (reg != reg_map.end() && reg->second != nullreg)
                fprintf(out_fp, "\tpushl %%%s\n", reg_to_str(reg->second));
              else
                fprintf(out_fp, "\tpushl %d(%%ebp)\n", offset_map.at(p));
            }
          }

          fprintf(out_fp, "\tcall %s\n", LLVMGetValueName(func));

          if (LLVMGetNumOperands(instr) >= 2)
            fprintf(out_fp, "\taddl $4, %%esp\n");

          fprintf(out_fp, "\tpopl %%edx\n");
          fprintf(out_fp, "\tpopl %%ecx\n");

          if (LLVMGetTypeKind(LLVMTypeOf(instr)) != LLVMVoidTypeKind) {
            auto reg = reg_map.find(instr);
            if (reg != reg_map.end() && reg->second != nullreg)
              fprintf(out_fp, "\tmovl %%eax, %%%s\n", reg_to_str(reg->second));
            else
              fprintf(out_fp, "\tmovl %%eax, %d(%%ebp)\n", offset_map.at(instr));
          }
          break;
        }

        case LLVMBr:
          if (LLVMGetNumOperands(instr) == 1) { // unconditional
            LLVMBasicBlockRef l = LLVMGetSuccessor(instr, 0);
            fprintf(out_fp, "\tjmp %s\n", label_map.at(l).c_str());
          } else { // conditional
            LLVMBasicBlockRef l1 = LLVMGetSuccessor(instr, 0);
            LLVMBasicBlockRef l2 = LLVMGetSuccessor(instr, 1);

            LLVMValueRef oper = LLVMGetOperand(instr, 0);
            LLVMIntPredicate t = LLVMGetICmpPredicate(oper);
            switch (t) {
            case LLVMIntEQ:
              fprintf(out_fp, "\tje %s\n", label_map.at(l1).c_str());
              break;
            case LLVMIntNE:
              fprintf(out_fp, "\tjne %s\n", label_map.at(l1).c_str());
              break;
            case LLVMIntSLT:
              fprintf(out_fp, "\tjl %s\n", label_map.at(l1).c_str());
              break;
            case LLVMIntSLE:
              fprintf(out_fp, "\tjle %s\n", label_map.at(l1).c_str());
              break;
            case LLVMIntSGT:
              fprintf(out_fp, "\tjg %s\n", label_map.at(l1).c_str());
              break;
            case LLVMIntSGE:
              fprintf(out_fp, "\tjge %s\n", label_map.at(l1).c_str());
              break;
            }

            fprintf(out_fp, "\tjmp %s\n", label_map.at(l2).c_str());
          }
          break;

        case LLVMAdd:
        case LLVMSub:
        case LLVMMul: {
          Register r = eax;
          int spill_flag = 0;
          auto reg = reg_map.find(instr);
          if (reg != reg_map.end() && reg->second != nullreg)
            r = reg->second;
          else
            spill_flag = 1;

          LLVMValueRef operA = LLVMGetOperand(instr, 0);
          if (LLVMIsAConstantInt(operA)) {
            int valA = (int)LLVMConstIntGetSExtValue(operA);
            fprintf(out_fp, "\tmovl $%d, %%%s\n", valA, reg_to_str(r));
          } else {
            auto regA = reg_map.find(operA);
            if (regA != reg_map.end() && regA->second != nullreg) {
              if (regA->second != r)
                fprintf(out_fp, "\tmovl %%%s, %%%s\n", reg_to_str(regA->second),
                        reg_to_str(r));
            } else {
              fprintf(out_fp, "\tmovl %d(%%ebp), %%%s\n", offset_map.at(operA),
                      reg_to_str(r));
            }
          }

          std::string opl;
          switch (opcode) {
          case LLVMAdd:
            opl = "addl";
            break;
          case LLVMSub:
            opl = "subl";
            break;
          case LLVMMul:
            opl = "imull";
            break;
          }

          LLVMValueRef operB = LLVMGetOperand(instr, 1);
          if (LLVMIsAConstantInt(operB)) {
            int valB = (int)LLVMConstIntGetSExtValue(operB);
            fprintf(out_fp, "\t%s $%d, %%%s\n", opl.c_str(), valB, reg_to_str(r));
          } else {
            auto regB = reg_map.find(operB);
            if (regB != reg_map.end() && regB->second != nullreg)
              fprintf(out_fp, "\t%s %%%s, %%%s\n", opl.c_str(),
                      reg_to_str(regB->second), reg_to_str(r));
            else
              fprintf(out_fp, "\t%s %d(%%ebp), %%%s\n", opl.c_str(),
                      offset_map.at(operB), reg_to_str(r));
          }

          if (spill_flag)
            fprintf(out_fp, "\tmovl %%eax, %d(%%ebp)\n", offset_map.at(instr));

          break;
        }

        case LLVMICmp: {
          Register r = eax;
          auto reg = reg_map.find(instr);
          if (reg != reg_map.end() && reg->second != nullreg)
            r = reg->second;

          LLVMValueRef operA = LLVMGetOperand(instr, 0);
          if (LLVMIsAConstantInt(operA)) {
            int valA = (int)LLVMConstIntGetSExtValue(operA);
            fprintf(out_fp, "\tmovl $%d, %%%s\n", valA, reg_to_str(r));
          } else {
            auto regA = reg_map.find(operA);
            if (regA != reg_map.end() && regA->second != nullreg) {
              if (regA->second != r)
                fprintf(out_fp, "\tmovl %%%s, %%%s\n", reg_to_str(regA->second),
                        reg_to_str(r));
            } else {
              fprintf(out_fp, "\tmovl %d(%%ebp), %%%s\n", offset_map.at(operA),
                      reg_to_str(r));
            }
          }

          LLVMValueRef operB = LLVMGetOperand(instr, 1);
          if (LLVMIsAConstantInt(operB)) {
            int valB = (int)LLVMConstIntGetSExtValue(operB);
            fprintf(out_fp, "\tcmpl $%d, %%%s\n", valB, reg_to_str(r));
          } else {
            auto regB = reg_map.find(operB);
            if (regB != reg_map.end() && regB->second != nullreg) {
              fprintf(out_fp, "\tcmpl %%%s, %%%s\n",
                      reg_to_str(regB->second), reg_to_str(r));
            } else {
              fprintf(out_fp, "\tcmpl %d(%%ebp), %%%s\n",
                      offset_map.at(operB), reg_to_str(r));
            }
          }

          break;
        }
        }
      }
    }
  }
}
