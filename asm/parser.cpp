#include "parser.hpp"
#include <cstdlib>

parse_tree_node * parse_instruction(lexer & token);
void parse_operand(lexer & token, std::vector<parse_tree_node *> & operands);
void parse_subscript(lexer & token, std::vector<parse_tree_node *> & operands);

static long int to_long_int(lexer & token){
    char * endptr;
    long int num = strtol(token.begin, &endptr, 0);
    if(endptr != token.end){
        throw parse_error("invalid number", token);
    }
    return num;
}

static bool next(lexer & token){
    
    bool has_next = token.next();
    
    // Skip over whitespace and comment tokens
    while(has_next && (token.type == WHITESPACE || token.type == COMMENT)){
        has_next = token.next();
    }
    
    return has_next;
}

parse_tree_node * parse(const char * begin, const char * end){
    
    lexer token(begin, end);
    
    parse_tree_node * head = NULL;
    parse_tree_node * tail = NULL;
    
    while(!token.at_end() && next(token)){
        
        switch(token.type){
            
            case NEWLINE:
                break;
            
            case WORD:{
                
                parse_tree_node * next = parse_instruction(token);
                
                if(tail){
                    next->insert(tail);
                }
                else{
                    head = next;
                }
                
                tail = next;
                
                if(!token.at_end() && token.type != NEWLINE){
                    parse_error::throw_unexpected(token);
                }
                
                break;
            }
            
            case LABEL:{
                
                if(!next(token) || token.type != WORD){
                    parse_error::throw_unexpected(token);
                }
                
                label * node = new label;
                node->identifier = token;
                
                if(tail){
                    node->insert(tail);
                }
                else{
                    head = node;
                }
                
                tail = node;
                
                break;
            }  
            default: parse_error::throw_unexpected(token);
        }
    }
    
    return head;
}

parse_tree_node * parse_instruction(lexer & token){
    
    lexer_token operation = token;
    std::vector<parse_tree_node *> operands;
    
    if(next(token) && token.type != NEWLINE){
        parse_operand(token, operands);
    }
    
    instruction * node = new instruction;
    node->operation = operation;
    node->operands = operands;
    return node;
}

void parse_operand(lexer & token, std::vector<parse_tree_node *> & operands){
    
    switch(token.type){
        case WORD:{
            symbol * node = new symbol;
            node->value = token;
            operands.push_back(node);
            break;
        }
        case NUMBER:{
            number * node = new number;
            node->value = token;
            operands.push_back(node);
            break;
        }
        case OPEN_SUBSCRIPT:
            parse_subscript(token, operands);
            break;
        default: parse_error::throw_unexpected(token);
    }
    
    if(next(token) && token.type == COMMA){
        if(next(token)){
            return parse_operand(token, operands);
        }
        else{
            parse_error::throw_unexpected(token);
        }
    }
}

void parse_subscript(lexer & token, std::vector<parse_tree_node *> & operands){
    
    if(!next(token)){
        parse_error::throw_unexpected(token);
    }
    
    if(token.type == WORD){
        
        subscript_symbol * node = new subscript_symbol;
        node->value = token;
        operands.push_back(node);
        
        if(!next(token) || token.type != CLOSE_SUBSCRIPT){
            parse_error::throw_unexpected(token);
        }
        
        return;
    }
    else{
        
        if(token.type != NUMBER){
            parse_error::throw_unexpected(token);
        }
        
        lexer_token number = token;
        
        if(!next(token)){
            parse_error::throw_unexpected(token);
        }
        
        if(token.type == PLUS){
            
            if(!next(token) || token.type != WORD){
                parse_error::throw_unexpected(token);
            }
            
            subscript_addition * node = new subscript_addition;
            node->number = number;
            node->symbol = token;
            operands.push_back(node);
            
            if(!next(token) || token.type != CLOSE_SUBSCRIPT){
                parse_error::throw_unexpected(token);
            }
            
            return;
        }
        else if(token.type != CLOSE_SUBSCRIPT){
            parse_error::throw_unexpected(token);
        }
        
        subscript_number * node = new subscript_number;
        node->value = number;
        operands.push_back(node);
        
        return;
    }
}

processor::word number::to_word()const{
    
    char * endptr;
    unsigned long num = strtoul(value.begin, &endptr, 0);
    
    if(endptr != value.end){
        throw parse_error("invalid number", error_location(value.line, 0));
    }
    
    return static_cast<processor::word>(num);
}

parse_tree_node::parse_tree_node()
:prev(NULL), next(NULL){
    
}

void parse_tree_node::insert(parse_tree_node * insert_after){
    
    // Disconnect from existing tree
    if(next || prev){
        
        if(next){
            next->prev = prev;
        }
        
        if(prev){
            prev->next = next;
        }
    }
    
    next = insert_after->next;
    prev = insert_after;
    
    insert_after->next = this;
    
    if(next){
        next->prev = this;
    }
}

void parse_tree_node::accept(parse_tree_node_visitor & visitor)const{
    
}

const parse_tree_node * parse_tree_node::get_prev()const{
    return prev;
}

const parse_tree_node * parse_tree_node::get_next()const{
    return next;
}

#define DEFINE_ACCEPT_METHOD(Class) \
    void Class::accept(parse_tree_node_visitor & visitor)const{visitor.visit(this);}

DEFINE_ACCEPT_METHOD(label)
DEFINE_ACCEPT_METHOD(instruction)
DEFINE_ACCEPT_METHOD(number)
DEFINE_ACCEPT_METHOD(symbol)
DEFINE_ACCEPT_METHOD(subscript_symbol)
DEFINE_ACCEPT_METHOD(subscript_number)
DEFINE_ACCEPT_METHOD(subscript_addition)

#define DEFINE_VISIT_METHOD(construct_class) \
    void parse_tree_node_visitor::visit(const construct_class *){}

DEFINE_VISIT_METHOD(label);
DEFINE_VISIT_METHOD(instruction);
DEFINE_VISIT_METHOD(symbol);
DEFINE_VISIT_METHOD(number);
DEFINE_VISIT_METHOD(subscript_symbol);
DEFINE_VISIT_METHOD(subscript_number);
DEFINE_VISIT_METHOD(subscript_addition);

