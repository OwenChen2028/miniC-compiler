#include "ir_builder.h"
#include "ast.h"
#include <string>
#include <unordered_map>
#include <vector>

std::vector<std::vector<std::string>> symbol_tables;
std::unordered_map<std::string, LLVMValueRef> var_map;

void walk_nodes(astNode *node) {
  if (!node) return;

  switch (node->type) {
  case ast_prog:
    walk_nodes(node->prog.ext1);
    walk_nodes(node->prog.ext2);
    walk_nodes(node->prog.func);
    break;

  case ast_func:
    symbol_tables.push_back(std::vector<std::string>());
    if (node->func.param)
      symbol_tables.back().push_back(node->func.param->var.name);
    if (node->func.body->stmt.block.stmt_list) {
      for (astNode *n : *node->func.body->stmt.block.stmt_list) {
        walk_nodes(n);
      }
    }
    symbol_tables.pop_back();
    break;

  case ast_extern:
    break;

  case ast_var: {
    
    break;
  }

  case ast_cnst:
    break;

  case ast_rexpr:
    walk_nodes(node->rexpr.lhs);
    walk_nodes(node->rexpr.rhs);
    break;

  case ast_bexpr:
    walk_nodes(node->bexpr.lhs);
    walk_nodes(node->bexpr.rhs);
    break;

  case ast_uexpr:
    walk_nodes(node->uexpr.expr);
    break;

  case ast_stmt:
    walk_stmt(&node->stmt);
    break;
  }
}

void walk_stmt(astStmt *stmt) {
  if (!stmt) return;

  switch (stmt->type) {
  case ast_call:
    walk_nodes(stmt->call.param);
    break;

  case ast_ret:
    walk_nodes(stmt->ret.expr);
    break;

  case ast_block:
    symbol_tables.push_back(std::vector<std::string>());
    if (stmt->block.stmt_list) {
      for (astNode *n : *stmt->block.stmt_list) {
        walk_nodes(n);
        if (error_code) break;
      }
    }
    symbol_tables.pop_back();
    break;

  case ast_while:
    walk_nodes(stmt->whilen.cond);
    walk_nodes(stmt->whilen.body);
    break;

  case ast_if:
    walk_nodes(stmt->ifn.cond);
    walk_nodes(stmt->ifn.if_body);
    walk_nodes(stmt->ifn.else_body);
    break;

  case ast_asgn:
    walk_nodes(stmt->asgn.lhs);
    walk_nodes(stmt->asgn.rhs);
    break;

  case ast_decl:
    for (int i = 0; i < (int)symbol_tables.back().size(); i++) {
      if (symbol_tables.back()[i] == stmt->decl.name) {
        error_code = 2;
        return;
      }
    }
    symbol_tables.back().push_back(stmt->decl.name);
    break;
  }
}

void preprocess(astNode *root) {
  walk_nodes(root);
}
