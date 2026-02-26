#include "ir_builder.h"
#include "ast.h"
#include <cstring>
#include <llvm-c/Core.h>
#include <string.h>
#include <string>
#include <unordered_map>
#include <unordered_set>
#include <vector>

std::vector<std::unordered_map<std::string, std::string>> renamings;
std::unordered_map<std::string, int> var_counter;

LLVMModuleRef module;

std::unordered_set<std::string> var_set;
std::unordered_map<std::string, LLVMValueRef> var_map;

LLVMValueRef func;
LLVMValueRef print_func;
LLVMValueRef read_func;

LLVMBuilderRef builder;

LLVMBasicBlockRef entryBB;
LLVMBasicBlockRef retBB;
LLVMBasicBlockRef currentBB;

LLVMValueRef ret_ref;

void preprocess_walk_stmt(astStmt *);

void preprocess_walk_nodes(astNode *node) {
  if (!node)
    return;

  switch (node->type) {
  case ast_prog:
    preprocess_walk_nodes(node->prog.ext1);
    preprocess_walk_nodes(node->prog.ext2);
    preprocess_walk_nodes(node->prog.func);
    break;

  case ast_func:
    renamings.emplace_back(std::unordered_map<std::string, std::string>());
    if (node->func.param) {
      std::string orig_name = node->func.param->var.name;
      node->func.param->var.name =
          strdup((orig_name + std::to_string(var_counter[orig_name])).c_str());
      renamings.back()[orig_name] = node->func.param->var.name;
      ++var_counter[orig_name];
      var_set.insert(node->func.param->var.name);
    }
    if (node->func.body->stmt.block.stmt_list) {
      for (astNode *n : *node->func.body->stmt.block.stmt_list) {
        preprocess_walk_nodes(n);
      }
    }
    renamings.pop_back();
    break;

  case ast_extern:
    break;

  case ast_var: {
    for (int i = (int)renamings.size() - 1; i >= 0; i--) {
      auto iter = renamings[i].find(node->var.name);
      if (iter != renamings[i].end()) {
        node->var.name = strdup(iter->second.c_str());
        break;
      }
    }
    break;
  }

  case ast_cnst:
    break;

  case ast_rexpr:
    preprocess_walk_nodes(node->rexpr.lhs);
    preprocess_walk_nodes(node->rexpr.rhs);
    break;

  case ast_bexpr:
    preprocess_walk_nodes(node->bexpr.lhs);
    preprocess_walk_nodes(node->bexpr.rhs);
    break;

  case ast_uexpr:
    preprocess_walk_nodes(node->uexpr.expr);
    break;

  case ast_stmt:
    preprocess_walk_stmt(&node->stmt);
    break;
  }
}

void preprocess_walk_stmt(astStmt *stmt) {
  if (!stmt)
    return;

  switch (stmt->type) {
  case ast_call:
    preprocess_walk_nodes(stmt->call.param);
    break;

  case ast_ret:
    preprocess_walk_nodes(stmt->ret.expr);
    break;

  case ast_block:
    renamings.push_back(std::unordered_map<std::string, std::string>());
    if (stmt->block.stmt_list) {
      for (astNode *n : *stmt->block.stmt_list) {
        preprocess_walk_nodes(n);
      }
    }
    renamings.pop_back();
    break;

  case ast_while:
    preprocess_walk_nodes(stmt->whilen.cond);
    preprocess_walk_nodes(stmt->whilen.body);
    break;

  case ast_if:
    preprocess_walk_nodes(stmt->ifn.cond);
    preprocess_walk_nodes(stmt->ifn.if_body);
    preprocess_walk_nodes(stmt->ifn.else_body);
    break;

  case ast_asgn:
    preprocess_walk_nodes(stmt->asgn.lhs);
    preprocess_walk_nodes(stmt->asgn.rhs);
    break;

  case ast_decl: {
    std::string orig_name = stmt->decl.name;
    stmt->decl.name =
        strdup((orig_name + std::to_string(var_counter[orig_name])).c_str());
    renamings.back()[orig_name] = stmt->decl.name;
    ++var_counter[orig_name];
    var_set.insert(stmt->decl.name);
    break;
  }
  }
}

void preprocess(astNode *root) {
  renamings.clear();
  var_counter.clear();
  var_map.clear();
  var_set.clear();
  preprocess_walk_nodes(root);
}

void genIRStmt(astStmt *stmt) {
  if (!stmt)
    return;

  switch (stmt->type) {
  case ast_asgn:
  
  break;

  
  }
}

LLVMValueRef genIRExpr(astNode *node) {
  
}

void build_ir(astNode *root) {
  preprocess(root);

  module = LLVMModuleCreateWithName("");
  LLVMSetTarget(module, "x86_64-pc-linux-gnu");

  LLVMTypeRef print_params[] = {LLVMInt32Type()};
  LLVMTypeRef print_type = LLVMFunctionType(LLVMVoidType(), print_params, 1, 0);
  print_func = LLVMAddFunction(module, "print", print_type);

  LLVMTypeRef read_type = LLVMFunctionType(LLVMInt32Type(), NULL, 0, 0);
  read_func = LLVMAddFunction(module, "read", read_type);

  astNode *node = root->prog.func;

  LLVMTypeRef func_params[] = {LLVMInt32Type()};
  LLVMTypeRef func_type =
      LLVMFunctionType(LLVMInt32Type(), node->func.param ? func_params : NULL,
                       node->func.param ? 1 : 0, 0);
  func = LLVMAddFunction(module, node->func.name, func_type);

  builder = LLVMCreateBuilder();

  entryBB = LLVMAppendBasicBlock(func, "entry");
  LLVMPositionBuilderAtEnd(builder, entryBB);

  var_map.clear();
  for (std::string var_name : var_set) { // includes func param
    LLVMValueRef alloc_stmt =
        LLVMBuildAlloca(builder, LLVMInt32Type(), var_name.c_str());
    var_map[var_name] = alloc_stmt;
  }

  // cannot collide with a variable name due to renaming
  ret_ref = LLVMBuildAlloca(builder, LLVMInt32Type(), "ret");

  LLVMBuildStore(builder, LLVMGetParam(func, 0),
                 var_map[node->func.param->var.name]);

  retBB = LLVMAppendBasicBlock(func, "return");
  LLVMPositionBuilderAtEnd(builder, retBB);

  LLVMValueRef r = LLVMBuildLoad2(builder, LLVMInt32Type(), ret_ref, "");
  LLVMBuildRet(builder, r);

  currentBB = entryBB;

  if (node->func.body->stmt.block.stmt_list) {
    for (astNode *n : *node->func.body->stmt.block.stmt_list) {
      genIRStmt(&n->stmt);
    }
  }

  LLVMValueRef terminator = LLVMGetBasicBlockTerminator(currentBB);
  if (!terminator) {
    LLVMPositionBuilderAtEnd(builder, currentBB);
    LLVMBuildBr(builder, retBB);
  }

  // remove all BB without pred
}
