#include "ir_builder.h"
#include "ast.h"
#include <string>
#include <unordered_map>
#include <vector>

std::vector<std::unordered_map<std::string, std::string>> renamings;
std::unordered_map<std::string, int> id_counter;

std::unordered_map<std::string, LLVMValueRef> var_map;

void walk_nodes(astNode *node) {
  if (!node)
    return;

  switch (node->type) {
  case ast_prog:
    walk_nodes(node->prog.ext1);
    walk_nodes(node->prog.ext2);
    walk_nodes(node->prog.func);
    break;

  case ast_func:
    renamings.emplace_back(std::unordered_map<std::string, std::string>());
    if (node->func.param) {
      std::string orig_name = node->func.param->var.name;
      node->func.param->var.name =
          orig_name + std::to_string(id_counter[orig_name]);
      renamings.back()[orig_name] = node->func.param->var.name;
      ++id_counter[orig_name];
    }
    if (node->func.body->stmt.block.stmt_list) {
      for (astNode *n : *node->func.body->stmt.block.stmt_list) {
        walk_nodes(n);
      }
    }
    renamings.pop_back();
    break;

  case ast_extern:
    break;

  case ast_var: {
    for (int i = (int)symbol_tables.size() - 1; i >= 0; i--) {
      if (symbol_tables[i].find(node->var.name) != symbol_tables[i].end()) {
        found = 1;
        break;
      }
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
  if (!stmt)
    return;

  switch (stmt->type) {
  case ast_call:
    walk_nodes(stmt->call.param);
    break;

  case ast_ret:
    walk_nodes(stmt->ret.expr);
    break;

  case ast_block:
    renamings.push_back(std::unordered_map<std::string, std::string>());
    if (stmt->block.stmt_list) {
      for (astNode *n : *stmt->block.stmt_list) {
        walk_nodes(n);
      }
    }
    renamings.pop_back();
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

  case ast_decl: {
    std::string orig_name = stmt->decl.name;
    stmt->decl.name = orig_name + std::to_string(id_counter[orig_name]);
    renamings.back()[orig_name] = stmt->decl.name;
    ++id_counter[orig_name];
    break;
  }
  }
}

void preprocess(astNode *root) {
  renamings.clear();
  id_counter.clear();
  var_map.clear();
  walk_nodes(root);
}

void build_ir(astNode *root) { preprocess(root); }
