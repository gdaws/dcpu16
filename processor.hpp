#ifndef DCPU16_PROCESSOR_HPP
#define DCPU16_PROCESSOR_HPP

#include "processor_exception.hpp"

class processor{
public:
    typedef unsigned short word;
    typedef unsigned long dword;
    
    typedef signed short signed_word;
    typedef signed long signed_dword;
    
    typedef processor_exception exception;
    
    static const unsigned int REGN = 8;
    
    enum BASIC_OPCODES{
        SET = 1, 
        ADD, 
        SUB, 
        MUL, 
        MLI,
        DIV, 
        DVI,
        MOD,
        MDI,
        AND,
        BOR,
        XOR,
        SHR, 
        ASR,
        SHL,
        IFB,
        IFC,
        IFE,
        IFN,
        IFG,
        IFA,
        IFL,
        IFU,
        ADX = 0x1a,
        SBX,
        STI = 0x1e,
        STD
    };
    
    enum NONBASIC_OPCODES{
        JSR = 1,
        HCF = 0x7,
        INT,
        IAG,
        IAS,
        HWN = 0x10,
        HWQ,
        HWI
    };
    
    enum REGISTER{
        A = 0, B, C, X, Y, Z, I, J
    };
    
    word reg[REGN];
    
    word * ram;
    word sp;
    word pc;
    word ex;
    
    processor();
    void run();
private:
    void run_nonbasic_instruction(word);
    
    template<int argument>
    word * decode_value(word, word *);
    
    bool perform_execution;
    word internal_reg[REGN];
};

#endif
