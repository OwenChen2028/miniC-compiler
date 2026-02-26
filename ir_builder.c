#include "ir_builder.h"
#include "ast.h"
#include <llvm-c/Core.h>
#include <string.h>
#include <string>
#include <unordered_map>
#include <unordered_set>
#include <vector>

std::vector<std::unordered_map<std::string, std::string>> renamings;
std::unordered_map<std::string, int> var_counter;

LLVMModuleRef llvm_module;

std::unordered_set<std::string> var_set;
std::unordered_map<std::string, LLVMValueRef> var_map;

LLVMValueRef print_func;
LLVMValueRef read_func;

LLVMBasicBlockRef entryBB;

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

void builder_walk_stmt(astStmt *);

void builder_walk_nodes(astNode *node) {
  if (!node)
    return;

  switch (node->type) {
  case ast_prog:
    llvm_module = LLVMModuleCreateWithName("");
    LLVMSetTarget(llvm_module, "x86_64-pc-linux-gnu");

    builder_walk_nodes(node->prog.ext1);
    builder_walk_nodes(node->prog.ext2);

    builder_walk_nodes(node->prog.func);
    break;

  case ast_func: {
    LLVMTypeRef params[] = {LLVMInt32Type()};
    LLVMTypeRef func_type =
        LLVMFunctionType(LLVMInt32Type(), node->func.param ? params : NULL,
                         node->func.param ? 1 : 0, 0);
    LLVMValueRef func =
        LLVMAddFunction(llvm_module, node->func.name, func_type);

    entryBB = LLVMAppendBasicBlock(func, "entry");

    LLVMBuilderRef builder = LLVMCreateBuilder();
    LLVMPositionBuilderAtEnd(builder, entryBB);

    var_map.clear();
    for (std::string var_name : var_set) {
      LLVMValueRef alloc_stmt =
          LLVMBuildAlloca(builder, LLVMInt32Type(), var_name.c_str());
      var_map[var_name] = alloc_stmt;
    }

    ret_ref =
        LLVMBuildAlloca(builder, LLVMInt32Type(),
                        "ret"); // no collision, var "ret" renamed to "ret0"

    break;
  }

  case ast_extern:
    if (node->ext.name == "print") {
      LLVMTypeRef params[] = {LLVMInt32Type()};
      LLVMTypeRef func_type = LLVMFunctionType(LLVMVoidType(), params, 1, 0);
      print_func = LLVMAddFunction(llvm_module, "print", func_type);
    } else if (node->ext.name == "read") {
      LLVMTypeRef func_type = LLVMFunctionType(LLVMInt32Type(), NULL, 0, 0);
      read_func = LLVMAddFunction(llvm_module, "read", func_type);
    }
    break;

  case ast_var: {
    break;
  }

  case ast_cnst:
    break;

  case ast_rexpr:
    builder_walk_nodes(node->rexpr.lhs);
    builder_walk_nodes(node->rexpr.rhs);
    break;

  case ast_bexpr:
    builder_walk_nodes(node->bexpr.lhs);
    builder_walk_nodes(node->bexpr.rhs);
    break;

  case ast_uexpr:
    builder_walk_nodes(node->uexpr.expr);
    break;

  case ast_stmt:
    builder_walk_stmt(&node->stmt);
    break;
  }
}

void builder_walk_stmt(astStmt *stmt) {
  if (!stmt)
    return;

  switch (stmt->type) {
  case ast_call:
    builder_walk_nodes(stmt->call.param);
    break;

  case ast_ret:
    builder_walk_nodes(stmt->ret.expr);
    break;

  case ast_block:
    break;

  case ast_while:
    builder_walk_nodes(stmt->whilen.cond);
    builder_walk_nodes(stmt->whilen.body);
    break;

  case ast_if:
    builder_walk_nodes(stmt->ifn.cond);
    builder_walk_nodes(stmt->ifn.if_body);
    builder_walk_nodes(stmt->ifn.else_body);
    break;

  case ast_asgn:
    builder_walk_nodes(stmt->asgn.lhs);
    builder_walk_nodes(stmt->asgn.rhs);
    break;

  case ast_decl: {
    break;
  }
  }
}

void build_ir(astNode *root) {
  preprocess(root);
  builder_walk_nodes(root);
}
