#include "code_generator.hpp"
#include "symbol_error.hpp"
#include "type_error.hpp"
#include <limits>
#include <vector>
#include <tuple>

enum ARGUMENT_NAME{
    ARG_A,
    ARG_B
};

class inline_data_generator:public parse_tree_node_visitor{
public:
    inline_data_generator(processor::word *& pc);
    virtual void visit(const number *);
    virtual void visit(const string *);
private:
    processor::word *& pc;
};

class code_generator:public parse_tree_node_visitor{
public:
    code_generator(const parse_tree_node * parse_result, symbol_table & symbols, processor::word * ram);
    
    virtual void visit(const label *);
    virtual void visit(const instruction *);
    virtual void visit(const symbol *);
    virtual void visit(const number *);
    virtual void visit(const subscript *);
    virtual void visit(const inline_data *);
    
    processor::word words_written()const;
    void write_label_addresses();
private:
    
    void encode_word_argument(processor::word, bool force_nextword = false);
    
    const parse_tree_node * parse_result;
    symbol_table & symbols;
    
    processor::word * ram;
    processor::word * pc;
    
    processor::word * operation;
    
    unsigned int argument_position;
    ARGUMENT_NAME argument_name;
    
    std::vector<std::tuple<const processor::word *, processor::word *>> label_references;
};

code_generator::code_generator(const parse_tree_node * _parse_result, symbol_table & _symbols, processor::word * _ram)
:parse_result(_parse_result), symbols(_symbols), ram(_ram), pc(_ram){
    
}

void code_generator::visit(const label * label){
    auto & label_symbol = symbols[label->identifier];
    std::get<1>(label_symbol) = pc - ram;
}

void code_generator::visit(const instruction * instruction){
    
    auto operation_symbol = symbols[instruction->operation];
    
    operation = pc++;
    
    switch(std::get<0>(operation_symbol)){
        case SYMBOL_TYPE_BASIC_INSTRUCTION:
            
            *operation = std::get<1>(operation_symbol);
            
            
            if(instruction->operands.size() != 2){
                throw type_error("basic instruction takes 2 values", error_location(instruction->operation));
            }
            
            argument_name = ARG_A;
            argument_position = 10;
            instruction->operands[1]->accept(*this);
            
            argument_name = ARG_B;
            argument_position = 5;
            instruction->operands[0]->accept(*this);
            
            break;
            
        case SYMBOL_TYPE_NONBASIC_INSTRUCTION:
            
            *operation = std::get<1>(operation_symbol) << 4;
            argument_position = 10;
            
            if(instruction->operands.size() != 1){
                throw type_error("non-basic instruction takes 1 value", error_location(instruction->operation));
            }
            
            argument_name = ARG_A;
            argument_position = 10;
            instruction->operands[0]->accept(*this);
            
            break;
        default:;
    }
}

void code_generator::visit(const symbol * symbol){
    
    auto & argument_symbol = symbols[symbol->value];
    
    switch(std::get<0>(argument_symbol)){
        
        case SYMBOL_TYPE_GENERAL_REGISTER:
        case SYMBOL_TYPE_SPECIAL_REGISTER:
        case SYMBOL_TYPE_STACK_OPERATION:
            *operation |= std::get<1>(argument_symbol) << argument_position;
            break;
        
        case SYMBOL_TYPE_LABEL:{
            
            const processor::word & address = std::get<1>(argument_symbol);
            
            if(address == label::ADDRESS_UNRESOLVED){
                encode_word_argument(0, true);
                label_references.push_back(std::forward_as_tuple(&address, pc - 1));
            }
            else{
                encode_word_argument(address);
            }
            
            break;
        }
        
        default:;
    }
}

void code_generator::encode_word_argument(processor::word value, bool force_nextword){
    
    if(value >= -1 && value <= 30 && !force_nextword && argument_name == ARG_A){
        *operation |= (0x20 + value) << argument_position;
    }
    else{
        *operation |= 0x1f << argument_position;
        *pc = value;
        pc++;
    }
}

void code_generator::write_label_addresses(){
    
    for(auto iterator: label_references){
        *std::get<1>(iterator) = *std::get<0>(iterator);
    }
}

void code_generator::visit(const number * number){
    processor::word word = number->to_word();
    encode_word_argument(word);
}

static void assign_subscript_argument(const lexer_token & arg, symbol_table & symbols, const lexer_token ** number, const lexer_token ** reg){
    
    if(arg.type == WORD){
        SYMBOL_TYPE arg_symbol_type = std::get<0>(symbols[arg]);
        if(arg_symbol_type == SYMBOL_TYPE_GENERAL_REGISTER){
            *reg = &arg;
        }
        else if(arg_symbol_type == SYMBOL_TYPE_LABEL){
            *number = &arg;   
        }
        else{
            throw symbol_error("symbol in subscript argument must be either a label or general register identifier", error_location(arg));
        }
    }
    else if(arg.type == NUMBER){
        *number = &arg;
    }
}

void code_generator::visit(const subscript * subscript){
    
    if(subscript->arguments.size() == 1){
        
        const lexer_token & arg = subscript->arguments[0];    
        
        switch(arg.type){
            case WORD:{
                
                auto & argument_symbol = symbols[arg];
                
                switch(std::get<0>(argument_symbol)){
                    
                    case SYMBOL_TYPE_GENERAL_REGISTER:
                        *operation |= (0x08 + std::get<1>(argument_symbol)) << argument_position;
                        break;
                    
                    case SYMBOL_TYPE_LABEL:{
                        
                        const processor::word & address = std::get<1>(argument_symbol);
                        
                        *operation |= 0x1e << argument_position;
                        
                        if(address == label::ADDRESS_UNRESOLVED){
                            label_references.push_back(std::forward_as_tuple(&address, pc));
                        }
                        else{
                            *pc = address;
                        }
                        
                        pc++;
                        
                        break;
                    }
                    
                    default:;
                }
                
                break;
            }
            case NUMBER:
                *operation |= 0x1e << argument_position;
                *pc = number::to_word(arg);
                pc++;
                break;
            default:;
        }
    }
    else{
        
        const lexer_token & arg1 = subscript->arguments[0];
        const lexer_token & arg2 = subscript->arguments[1];
        
        const lexer_token * next_word = NULL;
        const lexer_token * reg = NULL;
        
        assign_subscript_argument(arg1, symbols, &next_word, &reg);
        assign_subscript_argument(arg2, symbols, &next_word, &reg);
        
        if(next_word == NULL || reg == NULL || next_word == reg){
            throw parse_error("invalid subscript expression", error_location(arg1));
        }
        
        *operation |= (0x10 + std::get<1>(symbols[*reg])) << argument_position;
        
        if((*next_word).type == NUMBER){
            *pc = number::to_word(*next_word);
        }
        else{ // label
            
            const processor::word & address = std::get<1>(symbols[*next_word]);
            
            if(address == label::ADDRESS_UNRESOLVED){
                label_references.push_back(std::forward_as_tuple(&address, pc));
            }
            else{
                *pc = address;
            }
        }
        
        pc++;
    }
}

void code_generator::visit(const inline_data * inline_data){
    
    inline_data_generator generate_inline_data(pc);
    
    for(auto value: inline_data->values){
        value->accept(generate_inline_data);
    }
}

processor::word code_generator::words_written()const{
    return pc - ram;
}

inline_data_generator::inline_data_generator(processor::word * & _pc)
:pc(_pc){
    
}

void inline_data_generator::visit(const number * value){
    *(pc++) = value->to_word();
}

void inline_data_generator::visit(const string * string){
    const char * begin = string->value.begin + 1;
    const char * end = string->value.end - 1;
    for(const char * cur = begin; cur != end; cur++){
        *(pc++) = *cur;
    }
}

processor::word generate_code(const parse_tree_node * parse_result, symbol_table & symbols, processor::word * ram){
    
    code_generator code_generator(parse_result, symbols, ram);
    
    const parse_tree_node * node = parse_result;
    while(node){
        node->accept(code_generator);
        node = node->get_next();
    }
    
    code_generator.write_label_addresses();
    
    return code_generator.words_written();
}
