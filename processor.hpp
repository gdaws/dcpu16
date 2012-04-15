#ifndef PROCESSOR_HPP
#define PROCESSOR_HPP

#include "processor_exception.hpp"

class processor{
public:
    typedef unsigned short word;
    typedef unsigned long dword;
    typedef processor_exception exception;
    
    static const unsigned int REGN = 8;
    
    enum BASIC_OPCODES{
        SET = 1, 
        ADD, SUB, MUL, DIV, MOD,
        SHL, SHR, AND, BOR, XOR,
        IFE, IFN, IFG, IFB
    };
    
    enum NONBASIC_OPCODES{
        JSR = 1  
    };
    
    word reg[REGN];
    
    word * ram;
    word * sp;
    word * pc;
    word overflow;
        
    processor();
    void load_program();
    void run();
private:
    void run_nonbasic_instruction(word);
    word * decode_value(word, word *);
    
    bool perform_execution;
    word internal_reg[REGN];
};

#endif
