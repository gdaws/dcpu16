#ifndef DCPU16_ASM_CODE_GENERATOR_HPP
#define DCPU16_ASM_CODE_GENERATOR_HPP

#include "parser.hpp"
#include "symbol_table.hpp"
#include "../processor.hpp"

processor::word generate_code(const parse_tree_node * parse_result, symbol_table & symbols, processor::word * ram);

#endif
