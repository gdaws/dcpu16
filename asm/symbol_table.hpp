#ifndef DCPU16_ASM_SYMBOL_TABLE_HPP
#define DCPU16_ASM_SYMBOL_TABLE_HPP

#include "lexer.hpp"
#include "../processor.hpp"
#include <unordered_map>
#include <tuple>

enum SYMBOL_TYPE{
    BASIC_INSTRUCTION = 0,
    LABEL,
    REGISTER
};

typedef std::unordered_map<lexer_string, std::tuple<SYMBOL_TYPE, processor::word> > symbol_table;

void load_predefined_symbols(symbol_table &);

#endif
