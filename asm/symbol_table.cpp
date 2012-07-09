#include "symbol_table.hpp"
#include <cassert>

void load_predefined_symbols(symbol_table & st){
    
    static const char * basic_instructions[] = {
        "SET", "ADD", "SUB", "MUL", "DIV", "DVI",
        "MOD", "MDI", "AND", "BOR", "XOR", "SHR", 
        "ASR", "SHL", "IFB", "IFC", "IFE", "IFN", 
        "IFG", "IFA", "IFL", "IFU", "ADX", "SBX", 
        "STI", "STD"
    };
    
    for(unsigned int i = 0; i < sizeof(basic_instructions)/sizeof(char *); i++){
    
        st[lexer_string(basic_instructions[i])] = std::make_tuple(SYMBOL_TYPE_BASIC_INSTRUCTION, i + 1);
        
        // jump the reserved value gaps
        switch(i){
            case 0x17:
                i = 0x19;
                break;
            case 0x1b:
                i = 0x1d;
                break;
            default:
                break;
        }
    }
    
    static const char * nonbasic_instructions[] = {
        "JSR", "INT", "IAG", "IAS", "RFI", "IAQ", "HWN", "HWQ", "HWI"
    };
    
    for(unsigned int i = 0; i < sizeof(nonbasic_instructions)/sizeof(char *); i++){
        
        st[lexer_string(nonbasic_instructions[i])] = std::make_tuple(SYMBOL_TYPE_NONBASIC_INSTRUCTION, i + 1);
        
        // jump the reserved value gaps
        switch(i){
            case 0x01:
                i = 0x07;
                break;
            case 0x0c:
                i = 0x0f;
            default:
                break;
        }
    }
    
    static const char * general_registers[] = {
        "A", "B", "C", "X", "Y", "Z", "I", "J"
    };
    
    assert(sizeof(general_registers)/sizeof(char *) >= processor::REGN);
    
    for(unsigned int i = 0; i < processor::REGN; i++){
        st[lexer_string(general_registers[i])] = std::make_tuple(SYMBOL_TYPE_GENERAL_REGISTER, i);
    }
    
    st[lexer_string("PUSH")] = std::make_tuple(SYMBOL_TYPE_STACK_OPERATION, 0x18);
    st[lexer_string("POP")] = std::make_tuple(SYMBOL_TYPE_STACK_OPERATION, 0x18);
    st[lexer_string("PEEK")] = std::make_tuple(SYMBOL_TYPE_STACK_OPERATION, 0x19);
    st[lexer_string("PICK")] = std::make_tuple(SYMBOL_TYPE_STACK_OPERATION, 0x1a);
    
    static const char * special_registers[] = {
        "SP", "PC", "EX"  
    };
    
    for(unsigned int i = 0; i < sizeof(special_registers)/sizeof(char *); i++){
        st[lexer_string(special_registers[i])] = std::make_tuple(SYMBOL_TYPE_SPECIAL_REGISTER, i + 0x1b);
    }
}

