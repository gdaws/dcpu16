#include "symbol_table.hpp"
#include <cassert>

void load_predefined_symbols(symbol_table & st){
    
    static const char * basic_instructions[] = {
        "SET", "ADD", " SUB", " MUL", "DIV", "MOD", "SHL", " SHR", "AND", "BOR", "XOR", "IFE", "IFN", "IFG", "IFB"
    };
    
    for(unsigned int i = 0; i < sizeof(basic_instructions)/sizeof(char *); i++){
        st[lexer_string(basic_instructions[i])] = std::make_tuple(SYMBOL_TYPE_BASIC_INSTRUCTION, i + 1);
    }
    
    static const char * nonbasic_instructions[] = {
        "JSR"  
    };
    
    for(unsigned int i = 0; i < sizeof(nonbasic_instructions)/sizeof(char *); i++){
        st[lexer_string(nonbasic_instructions[i])] = std::make_tuple(SYMBOL_TYPE_NONBASIC_INSTRUCTION, i + 1);
    }
    
    static const char * general_registers[] = {
        "A", "B", "C", "X", "Y", "Z", "I", "J"
    };
    
    assert(sizeof(general_registers)/sizeof(char *) > processor::REGN);
    
    for(unsigned int i = 0; i < processor::REGN; i++){
        st[lexer_string(general_registers[i])] = std::make_tuple(SYMBOL_TYPE_GENERAL_REGISTER, i);
    }
    
    static const char * special_registers[] = {
        "SP", "PC", "O"  
    };
    
    for(unsigned int i = 0; i < sizeof(special_registers)/sizeof(char *); i++){
        st[lexer_string(special_registers[i])] = std::make_tuple(SYMBOL_TYPE_SPECIAL_REGISTER, i + 0x1b);
    }
    
    static const char * stack_operations[] = {
        "POP", "PEEK", "PUSH"  
    };
    
    for(unsigned int i = 0; i < sizeof(stack_operations)/sizeof(char *); i++){
        st[lexer_string(stack_operations[i])] = std::make_tuple(SYMBOL_TYPE_STACK_OPERATION, i + 0x18);
    }
    
}

