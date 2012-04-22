#include "symbol_analyser.hpp"
#include "symbol_error.hpp"
#include <sstream>
#include <limits>

template<typename MapIterator>
static SYMBOL_TYPE symbol_type(MapIterator iter){
    return std::get<0>(std::get<1>(*iter));
}

class labels_visitor:public parse_tree_node_visitor{
public:
    labels_visitor(symbol_table & symbol);
    virtual void visit(const label *);
private:
    symbol_table & symbols;
};

labels_visitor::labels_visitor(symbol_table & symbols)
:symbols(symbols){
    
}

void labels_visitor::visit(const label * label){
    
    auto existing_type = symbols.find(label->identifier);
    
    if(symbols.end() != existing_type){
        
        if(symbol_type(existing_type) == SYMBOL_TYPE_LABEL){
            
            std::stringstream what;
            what << "'" << std::string(label->identifier.begin, label->identifier.end) << "' is already used by a previously declared label";
            
            throw symbol_error(what.str(), error_location(label->identifier));
        }
        else{
            std::stringstream what;
            what << "cannot use reserved symbol '" << std::string(label->identifier.begin, label->identifier.end) << "' as a label identifier";
            
            throw symbol_error(what.str(), error_location(label->identifier));
        }
    }
    
    symbols[label->identifier] = std::make_tuple(SYMBOL_TYPE_LABEL, label::ADDRESS_UNRESOLVED);
}

class check_symbols_visitor:public parse_tree_node_visitor{
public:
    check_symbols_visitor(symbol_table & symbol);
    
    virtual void visit(const instruction *);
    virtual void visit(const symbol *);
    virtual void visit(const subscript_symbol *);
    virtual void visit(const subscript_addition *);
private:
    void throw_unknown_symbol(const lexer_token & str);
    void throw_wrong_symbol_type(const lexer_token & str, SYMBOL_TYPE expected_type);
    
    symbol_table & symbols;
};

check_symbols_visitor::check_symbols_visitor(symbol_table & symbols)
:symbols(symbols){
    
}

void check_symbols_visitor::visit(const instruction * instruction){
    
    auto operation_type = symbols.find(instruction->operation);
    
    if(symbols.end() == operation_type){
        throw_unknown_symbol(instruction->operation);
    }
    
    if(symbol_type(operation_type) != SYMBOL_TYPE_BASIC_INSTRUCTION && symbol_type(operation_type) != SYMBOL_TYPE_NONBASIC_INSTRUCTION){
        throw_wrong_symbol_type(instruction->operation, SYMBOL_TYPE_BASIC_INSTRUCTION);
    }
    
    for(unsigned int i = 0; i < instruction->operands.size(); i++){
        instruction->operands[i]->accept(*this);    
    }
}

void check_symbols_visitor::visit(const symbol * symbol){
    
    auto symbol_entry = symbols.find(symbol->value);
    
    if(symbols.end() == symbol_entry){
        throw_unknown_symbol(symbol->value);
    }
}

void check_symbols_visitor::visit(const subscript_symbol * symbol){
    
    auto symbol_entry = symbols.find(symbol->value);
    
    if(symbols.end() == symbol_entry){
        throw_unknown_symbol(symbol->value);
    }
}

void check_symbols_visitor::visit(const subscript_addition * addition){
    
    auto symbol_entry = symbols.find(addition->symbol);
    
    if(symbols.end() == symbol_entry){
        throw_unknown_symbol(addition->symbol);
    }
}

void check_symbols_visitor::throw_unknown_symbol(const lexer_token & token){
    
    std::stringstream what;
    what << "unknown symbol '" << std::string(token.begin, token.end) <<"'";
    
    throw symbol_error(what.str(), error_location(token));
}

void check_symbols_visitor::throw_wrong_symbol_type(const lexer_token & token, SYMBOL_TYPE type){
    
    std::stringstream what;
    what << "'" << std::string(token.begin, token.end) <<"' ";
    
    switch(type){
        case SYMBOL_TYPE_BASIC_INSTRUCTION:
        case SYMBOL_TYPE_NONBASIC_INSTRUCTION:
            what << "is not an instruction operation symbol";
            break;
        default:
            what << "is an unknown symbol";    
    }
    
    throw symbol_error(what.str(), error_location(token));
}

void load_symbols(const parse_tree_node * parse_result, symbol_table & symbols){
    
    const parse_tree_node * node = parse_result;
    
    labels_visitor load_userdefined_symbols(symbols);
    
    while(node){
        node->accept(load_userdefined_symbols);
        node = node->get_next();
    }
    
    check_symbols_visitor check_symbols(symbols);
    
    node = parse_result;
    while(node){
        node->accept(check_symbols);
        node = node->get_next();
    }
}
