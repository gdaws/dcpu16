#include "parser.hpp"
#include <cstdlib>

static parse_tree_node * parse_instruction(lexer & token);
static parse_tree_node * parse_inline_data(lexer & token);

static void parse_operand(lexer & token, std::vector<parse_tree_node *> & operands);
static void parse_subscript(lexer & token, std::vector<parse_tree_node *> & operands);
static void parse_inline_data_value(lexer & token, std::vector<parse_tree_node *> & operands);

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
                
                parse_tree_node * next = (token == "DAT" ? parse_inline_data(token) : parse_instruction(token));
                
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

static parse_tree_node * parse_instruction(lexer & token){
    
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

static void parse_operand(lexer & token, std::vector<parse_tree_node *> & operands){
    
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

static void parse_subscript_argument(lexer & token, subscript * node){
    
    switch(token.type){
        case WORD:
        case NUMBER:
            node->arguments.push_back(token);
            break;
        default:parse_error::throw_unexpected(token);
    }
        
    if(next(token) && token.type == PLUS){
        if(next(token)){
            return parse_subscript_argument(token, node);
        }
        else{
            parse_error::throw_unexpected(token);   
        }
    }
}

static void parse_subscript(lexer & token, std::vector<parse_tree_node *> & operands){
    
    if(!next(token)){
        parse_error::throw_unexpected(token);
    }
    
    subscript * node = new subscript;
    
    parse_subscript_argument(token, node);
    
    if(token.type != CLOSE_SUBSCRIPT){
        parse_error::throw_unexpected(token);
    }
    
    if(node->arguments.size() > 2){
        throw parse_error("too many operands in subscript", error_location(token));
    }
    
    operands.push_back(node);
}

static parse_tree_node * parse_inline_data(lexer & token){
    
    std::vector<parse_tree_node *> values;
    
    if(next(token) && token.type != NEWLINE){
        parse_inline_data_value(token, values);
    }
    
    inline_data * node = new inline_data;
    node->values = values;
    return node;
}

static void parse_inline_data_value(lexer & token, std::vector<parse_tree_node *> & operands){
    
    switch(token.type){
        case NUMBER:{
            number * node = new number;
            node->value = token;
            operands.push_back(node);
            break;
        }
        case STRING:{
            string * node = new string;
            node->value = token;
            operands.push_back(node);
            break;
        }
        default: parse_error::throw_unexpected(token);
    }
    
    if(next(token) && token.type == COMMA){
        if(next(token)){
            return parse_inline_data_value(token, operands);
        }
        else{
            parse_error::throw_unexpected(token);
        }
    }
}

processor::word number::to_word(const lexer_token & token){
    
    char * endptr;
    unsigned long num = strtoul(token.begin, &endptr, 0);
    
    if(endptr != token.end){
        throw parse_error("invalid number", error_location(token.line, token.column));
    }
    
    return static_cast<processor::word>(num);
}

processor::word number::to_word()const{
    return to_word(value);
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

const processor::word label::ADDRESS_UNRESOLVED = std::numeric_limits<processor::word>::max();

#define DEFINE_ACCEPT_METHOD(Class) \
    void Class::accept(parse_tree_node_visitor & visitor)const{visitor.visit(this);}

DEFINE_ACCEPT_METHOD(label)
DEFINE_ACCEPT_METHOD(instruction)
DEFINE_ACCEPT_METHOD(number)
DEFINE_ACCEPT_METHOD(symbol)
DEFINE_ACCEPT_METHOD(subscript)
DEFINE_ACCEPT_METHOD(string)
DEFINE_ACCEPT_METHOD(inline_data)

#define DEFINE_VISIT_METHOD(construct_class) \
    void parse_tree_node_visitor::visit(const construct_class *){}

DEFINE_VISIT_METHOD(label)
DEFINE_VISIT_METHOD(instruction)
DEFINE_VISIT_METHOD(symbol)
DEFINE_VISIT_METHOD(number)
DEFINE_VISIT_METHOD(subscript)
DEFINE_VISIT_METHOD(string)
DEFINE_VISIT_METHOD(inline_data)
