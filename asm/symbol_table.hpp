#ifndef DCPU16_ASM_SYMBOL_TABLE_HPP
#define DCPU16_ASM_SYMBOL_TABLE_HPP

#include "lexer.hpp"
#include "../processor.hpp"
#include <unordered_map>
#include <tuple>

enum SYMBOL_TYPE{
    SYMBOL_TYPE_BASIC_INSTRUCTION = 0,
    SYMBOL_TYPE_NONBASIC_INSTRUCTION,
    SYMBOL_TYPE_LABEL,
    SYMBOL_TYPE_GENERAL_REGISTER,
    SYMBOL_TYPE_SPECIAL_REGISTER,
    SYMBOL_TYPE_STACK_OPERATION
};

typedef std::unordered_map<lexer_string, std::tuple<SYMBOL_TYPE, processor::word>> symbol_table;

void load_predefined_symbols(symbol_table &);

#endif
