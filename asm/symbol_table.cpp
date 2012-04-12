#include "symbol_table.hpp"
#include <cassert>

void load_predefined_symbols(symbol_table & st){
    
    static const char * basic_instructions[] = {
        "SET", "ADD", " SUB", " MUL", "DIV", "MOD", "SHL", " SHR", "AND", "BOR", "XOR", "IFE", "IFN", "IFG", "IFB"
    };
    
    for(int i = 0; i < sizeof(basic_instructions)/sizeof(char *); i++){
        st[lexer_string(instructions[i])] = std::make_tuple(INSTRUCTION, i + 1);
    }
    
    static const char * register_symbols[] = {
        "A", "B", "C", "D", "E", "F", "G", "H", "I", "J", "K", "L", "M", "N"  
    };
    
    assert(sizeof(register_symbol)/sizeof(char *) > processor::REGN);
    
    for(int i = 0; i < processor::REGN; i++){
        st[lexer_string(register_symbol[i])] = std::make_tuple(REGISTER, i);
    }
    
}
