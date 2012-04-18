#ifndef DCPU16_ASM_SYMBOL_ANALYSER_HPP
#define DCPU16_ASM_SYMBOL_ANALYSER_HPP

#include "parser.hpp"
#include "lexer.hpp"
#include "symbol_table.hpp"

void load_symbols(const parse_tree_node * parse_result, symbol_table & symbols);

#endif
