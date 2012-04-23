#include "processor.hpp"
#include <limits>
#include <iostream>

processor::processor()
:perform_execution(true){
    
}

void processor::run(){
    
    word instruction = ram[pc++];
    
    word opcode = instruction & 0xf;
    
    if(opcode == 0x0){
        return run_nonbasic_instruction(instruction);
    }
    
    word & a = *decode_value((instruction >> 0x04) & 0x3f, internal_reg + 0);
    word & b = *decode_value(instruction >> 0x0a, internal_reg + 1);
    
    if(!perform_execution){
        perform_execution = true;
        return;
    }
    
    const int WORD_DIGITS = std::numeric_limits<word>::digits;
    const int WORD_MAX = std::numeric_limits<word>::max();
    
    switch(static_cast<BASIC_OPCODES>(opcode)){
        case SET:
            a = b;
            return;
        case ADD:
            internal_reg[3] = a;
            a = a + b;
            overflow = a < internal_reg[3];
            return;
        case SUB:
            internal_reg[3] = a;
            a = a - b;
            overflow = -1 * (a > internal_reg[3]);
            return;
        case MUL:{
            dword c = static_cast<dword>(a) * static_cast<dword>(b);
            a = c & WORD_MAX;
            overflow = c >> WORD_DIGITS;
            return;
        }
        case DIV:{
            dword c = static_cast<dword>(a) << WORD_DIGITS;
            overflow = c / b;
            a = a / b;
            return;
        }
        case MOD:
            a = a % b;
            return;
        case SHL:{
            dword c = static_cast<dword>(a) << static_cast<dword>(b);
            a = c & WORD_MAX;
            overflow = c >> WORD_DIGITS;
            return;
        }
        case SHR:{
            a = a >> b;
            overflow = ((static_cast<dword>(a) << WORD_DIGITS) >> b) & WORD_DIGITS;
            return;
        }
        case AND:
            a  = a & b;
            return;
        case BOR:
            a = a | b;
            return;
        case XOR:
            a = a ^ b;
            return;
        case IFE:
            perform_execution = a == b;
            return;
        case IFN:
            perform_execution = a != b;
            return;
        case IFG:
            perform_execution = a > b;
            return;
        case IFB:
            perform_execution = (a & b) != 0;
            return;
        default:
            throw processor::exception("unrecognized opcode (in basic instruction)");
    }
}

void processor::run_nonbasic_instruction(word instruction){
    
    word opcode = (instruction >> 4) & 0xf;
    
    if(opcode == 0x0){
        throw processor::exception("unrecognized opcode (in extended non-basic instruction)");
    }
    
    word & a = *decode_value(instruction >> 0xa, internal_reg + 0);
    
    if(!perform_execution){
        perform_execution = true;
        return;
    }
    
    switch(static_cast<NONBASIC_OPCODES>(opcode)){
        case JSR:
            ram[--sp] = pc;
            pc = a;
            return;
        default:
            throw processor::exception("unrecognized opcode (in non-basic instruction)");
    }
    
}

processor::word * processor::decode_value(word ref, word * store){
    
    switch(ref){
        
        case 0x00: case 0x01: case 0x02: case 0x03: case 0x04: case 0x05: case 0x06: case 0x07:
            return reg + ref;
        
        case 0x08: case 0x09: case 0x0a: case 0x0b: case 0x0c: case 0x0d: case 0x0e: case 0x0f:
            return ram + reg[ref - 0x08];
        
        case 0x10: case 0x11: case 0x12: case 0x13: case 0x14: case 0x15: case 0x16: case 0x17:
            return ram + (pc++ + reg[ref - 0x10]);
        
        case 0x18:
            return ram + sp++;
        
        case 0x19:
            return ram + sp;
        
        case 0x1a:
            return ram + --sp;
        
        case 0x1b:
            return &sp;
            
        case 0x1c:
            return &pc;
        
        case 0x1d:
            return &overflow;
        
        case 0x1e:
            return ram + pc++;
        
        case 0x1f:
            *store = ram[pc++];
            return store;
        
        case 0x20: case 0x21: case 0x22: case 0x23: case 0x24: case 0x25: case 0x26: case 0x27:
        case 0x28: case 0x29: case 0x2a: case 0x2b: case 0x2c: case 0x2d: case 0x2e: case 0x2f:
        case 0x30: case 0x31: case 0x32: case 0x33: case 0x34: case 0x35: case 0x36: case 0x37:
        case 0x38: case 0x39: case 0x3a: case 0x3b: case 0x3c: case 0x3d: case 0x3e: case 0x3f:
            *store = ref - 0x20;
            return store;
    }
    
    throw processor::exception("value decode error");
}
