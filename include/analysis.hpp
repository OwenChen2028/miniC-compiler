#pragma once

#include "ast.hpp"

/*
0 - no errors found
1 - variable used before declaration
2 - variable declared twice in one scope
*/
int semantic_analysis(astNode *root);
