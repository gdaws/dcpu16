#ifndef DCPU16_PROCESSOR_HPP
#define DCPU16_PROCESSOR_HPP

#include <deque>
#include <tuple>
#include "processor_exception.hpp"

class device;

class processor{
public:
    typedef unsigned short word;
    typedef unsigned long dword;
    
    typedef signed short signed_word;
    typedef signed long signed_dword;
    
    typedef processor_exception exception;
    
    static const unsigned int REGN = 8;
    static const unsigned int MAX_INTERRUPT_QUEUE_SIZE = 256;
    
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
        INT = 0x8,
        IAG,
        IAS,
        RFI,
        IAQ,
        HWN = 0x10,
        HWQ,
        HWI
    };
    
    enum REGISTER{
        A = 0, B, C, X, Y, Z, I, J
    };
    
    word * ram;
    
    device ** devices;
    word devices_connected;
    
    word sp;
    word pc;
    word ex;
    word ia;
    word reg[REGN];
    
    processor();
    void run();
private:
    void run_basic_instruction(word);
    void run_nonbasic_instruction(word);
    
    void setup_interrupt_handler(word, word);
    void setup_interrupt_handler(const std::tuple<word, word> &);
    
    template<int argument>
    word * decode_value(word, word *);
    
    bool perform_execution;
    
    bool interrupt_queue_enabled;
    std::deque< std::tuple<word, word> > interrupt_queue;
    
    word internal_reg[REGN];
};

#endif
