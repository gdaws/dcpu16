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
    
    word & a = *decode_value<1>(instruction >> 10, internal_reg + 0);
    word & b = *decode_value<2>((instruction >> 5) & 0x1f, internal_reg + 1);
    
    if(!perform_execution){
        perform_execution = true;
        return;
    }
    
    const int WORD_DIGITS = std::numeric_limits<word>::digits;
    const int WORD_MAX = std::numeric_limits<word>::max();
    
    switch(static_cast<BASIC_OPCODES>(opcode)){
        case SET:
            b = a;
            return;
        
        case ADD:
            internal_reg[3] = b;
            b = a + b;
            ex = a < internal_reg[3];
            return;
        
        case SUB:
            internal_reg[3] = b;
            b = b - a;
            ex = -1 * (b > internal_reg[3]);
            return;
        
        case MUL:{
            dword c = a * b;
            b = c;
            ex = c >> WORD_DIGITS;
            return;
        }
        
        case MLI:{
            
            signed_dword ai = a;
            signed_dword bi = b;
            signed_dword ci = ai * bi;
            
            b = ci;
            ex = ci >> WORD_DIGITS;
            
            return;
        }
        
        case DIV:
        
            if(a != 0){
                b = b / a;
                ex = (static_cast<dword>(b) << WORD_DIGITS) / a;
            }
            else{
                b = 0;
                ex = 0;
            }
            
            return;
        
        case DVI:{
            
            if(a != 0){
                
                signed_dword ai = a;
                signed_dword bi = b;
                
                b = bi / ai;
                ex = (bi << WORD_DIGITS) / a;
            }
            else{
                b = 0;
                ex = 0;
            }
            
            return;
            
        case MOD:
            b = (a != 0 ? b % a : 0);
            return;
        
        case MDI:{
            
            signed_word ai = a;
            signed_word bi = bi;
            
            b = (ai != 0 ? bi % ai : 0);
            
            return;
        }
        
        case AND:
            b = b & a;
            return;
        
        case BOR:
            b = b | a;
            return;
        
        case XOR:
            b = b ^ a;
            return;
        
        case SHR:{
            b =  b >> a;
            ex = (static_cast<dword>(b) << WORD_DIGITS) >> a;
            return;
        }
        
        case ASR:{
            signed_word bi = b;
            b = bi >> a;
            ex = (bi << WORD_DIGITS) >> a;
        }
        
        case SHL:{
            dword c = b << a;
            b = c;
            ex = c >> WORD_DIGITS;
            return;
        }
        

        case IFB:
            perform_execution = (b & a) != 0;
            return;
        
        case IFC:
            perform_execution = (b & a) == 0;
            return;
        
        case IFE:
            perform_execution = a == b;
            return;
        
        case IFN:
            perform_execution = a != b;
            return;
        
        case IFG:
            perform_execution = b > a;
            return;
        
        case IFA:
            perform_execution = static_cast<signed_word>(b) > static_cast<signed_word>(a);
            return;
            
        case IFL:
            perform_execution = b < a;
            return;
        
        case IFU:
            perform_execution = static_cast<signed_word>(b) < static_cast<signed_word>(a);
            return;
        
        case ADX:{     
            dword c = b + a + ex;
            b = c;
            ex = c > WORD_MAX;
            return;
        }
            
        case SBX:{
                
        }
        
        case STI:
            b = a;
            reg[I]++;
            reg[J]++;
            return;
        
        case STD:
            b = a;
            reg[I]--;
            reg[J]--;
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
    
    word & a = *decode_value<1>(instruction >> 0xa, internal_reg + 0);
    
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

template<int argument>
processor::word * processor::decode_value(word value, word * temp_value){
    
    switch(value){
        
        case 0x00: case 0x01: case 0x02: case 0x03: case 0x04: case 0x05: case 0x06: case 0x07:
            return reg + value;
        
        case 0x08: case 0x09: case 0x0a: case 0x0b: case 0x0c: case 0x0d: case 0x0e: case 0x0f:
            return ram + reg[value - 0x08];
        
        case 0x10: case 0x11: case 0x12: case 0x13: case 0x14: case 0x15: case 0x16: case 0x17:
            return ram + (pc++ + reg[value - 0x10]);
        
        case 0x18:
        
            if(argument == 1){
                return ram + sp++;
            }
            
            if(argument == 2){
                return ram + --sp;
            }
            
            throw processor::exception("value decode error");
            
        case 0x19:
            return ram + sp;
        
        case 0x1a:
            return ram + static_cast<processor::word>(sp + pc++);
        
        case 0x1b:
            return &sp;
        
        case 0x1c:
            return &pc;
        
        case 0x1d:
            return &ex;
        
        case 0x1e:
            return ram + pc++;
        
        case 0x1f:
            *temp_value = ram[pc++];
            return temp_value;
        
        case 0x20: case 0x21: case 0x22: case 0x23: case 0x24: case 0x25: case 0x26: case 0x27:
        case 0x28: case 0x29: case 0x2a: case 0x2b: case 0x2c: case 0x2d: case 0x2e: case 0x2f:
        case 0x30: case 0x31: case 0x32: case 0x33: case 0x34: case 0x35: case 0x36: case 0x37:
        case 0x38: case 0x39: case 0x3a: case 0x3b: case 0x3c: case 0x3d: case 0x3e: case 0x3f:
            *temp_value = value - 0x20;
            return temp_value;
    }
    
    throw processor::exception("value decode error");
}
