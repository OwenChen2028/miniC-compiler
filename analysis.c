#include "analysis.h"
#include "ast.h"
#include <string>
#include <unordered_set>
#include <vector>

std::vector<std::unordered_set<std::string>> symbol_tables;
int error_code = 0;

void walk_stmt(astStmt *stmt);

void walk_nodes(astNode *node) {
  if (!node || error_code) return;

  switch (node->type) {
  case ast_prog:
    walk_nodes(node->prog.ext1);
    walk_nodes(node->prog.ext2);
    walk_nodes(node->prog.func);
    break;

  case ast_func:
    symbol_tables.push_back(std::unordered_set<std::string>());
    if (node->func.param)
      symbol_tables.back().insert(node->func.param->var.name);
    if (node->func.body->stmt.block.stmt_list) {
      for (astNode *n : *node->func.body->stmt.block.stmt_list) {
        walk_nodes(n);
        if (error_code) break;
      }
    }
    symbol_tables.pop_back();
    break;

  case ast_extern:
    break;

  case ast_var: {
    int found = 0;
    for (int i = (int)symbol_tables.size() - 1; i >= 0; i--) {
      if (symbol_tables[i].find(node->var.name) != symbol_tables[i].end()) {
        found = 1;
        break;
      }
      if (found) break;
    }
    if (!found) {
      error_code = 1;
      return;
    }
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
  if (!stmt || error_code) return;

  switch (stmt->type) {
  case ast_call:
    walk_nodes(stmt->call.param);
    break;

  case ast_ret:
    walk_nodes(stmt->ret.expr);
    break;

  case ast_block:
    symbol_tables.push_back(std::unordered_set<std::string>());
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
    if (symbol_tables.back().find(stmt->decl.name) != symbol_tables.back().end()) {
      error_code = 2;
      return;
    }
    symbol_tables.back().insert(stmt->decl.name);
    break;
  }
}

/*
0 - no errors found
1 - variable used before declaration
2 - variable declared twice in one scope
*/
int semantic_analysis(astNode *root) {
  symbol_tables.clear();
  error_code = 0;
  symbol_tables.push_back(std::unordered_set<std::string>());
  walk_nodes(root);
  symbol_tables.pop_back();
  return error_code;
}
