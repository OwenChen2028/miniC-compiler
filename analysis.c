#include "analysis.h"
#include "ast.h"
#include <string>
#include <unordered_set>
#include <vector>

std::vector<std::unordered_set<std::string>> symbol_tables;
int error_code = 0;

void analysis_walk_stmt(astStmt *);

void analysis_walk_nodes(astNode *node) {
  if (!node || error_code)
    return;

  switch (node->type) {
  case ast_prog:
    analysis_walk_nodes(node->prog.ext1);
    analysis_walk_nodes(node->prog.ext2);
    analysis_walk_nodes(node->prog.func);
    break;

  case ast_func:
    symbol_tables.emplace_back(std::unordered_set<std::string>());
    if (node->func.param)
      symbol_tables.back().insert(node->func.param->var.name);
    if (node->func.body->stmt.block.stmt_list) {
      for (astNode *n : *node->func.body->stmt.block.stmt_list) {
        analysis_walk_nodes(n);
        if (error_code)
          break;
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
    analysis_walk_nodes(node->rexpr.lhs);
    analysis_walk_nodes(node->rexpr.rhs);
    break;

  case ast_bexpr:
    analysis_walk_nodes(node->bexpr.lhs);
    analysis_walk_nodes(node->bexpr.rhs);
    break;

  case ast_uexpr:
    analysis_walk_nodes(node->uexpr.expr);
    break;

  case ast_stmt:
    analysis_walk_stmt(&node->stmt);
    break;
  }
}

void analysis_walk_stmt(astStmt *stmt) {
  if (!stmt || error_code)
    return;

  switch (stmt->type) {
  case ast_call:
    analysis_walk_nodes(stmt->call.param);
    break;

  case ast_ret:
    analysis_walk_nodes(stmt->ret.expr);
    break;

  case ast_block:
    symbol_tables.emplace_back(std::unordered_set<std::string>());
    if (stmt->block.stmt_list) {
      for (astNode *n : *stmt->block.stmt_list) {
        analysis_walk_nodes(n);
        if (error_code)
          break;
      }
    }
    symbol_tables.pop_back();
    break;

  case ast_while:
    analysis_walk_nodes(stmt->whilen.cond);
    analysis_walk_nodes(stmt->whilen.body);
    break;

  case ast_if:
    analysis_walk_nodes(stmt->ifn.cond);
    analysis_walk_nodes(stmt->ifn.if_body);
    analysis_walk_nodes(stmt->ifn.else_body);
    break;

  case ast_asgn:
    analysis_walk_nodes(stmt->asgn.lhs);
    analysis_walk_nodes(stmt->asgn.rhs);
    break;

  case ast_decl:
    if (symbol_tables.back().find(stmt->decl.name) !=
        symbol_tables.back().end()) {
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
  analysis_walk_nodes(root);
  return error_code;
}
