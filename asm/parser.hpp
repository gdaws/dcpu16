#ifndef DCPU16_ASM_PARSER_HPP
#define DCPU16_ASM_PARSER_HPP

#include "parse_error.hpp"
#include "lexer.hpp"
#include "../processor.hpp"
#include <vector>
#include <stdexcept>
#include <tuple>
#include <limits>

class label;
class instruction;
class number;
class symbol;
class subscript_symbol;
class subscript_number;
class subscript_addition;

class parse_tree_node_visitor{
public:
    virtual void visit(const label *);
    virtual void visit(const instruction *);
    virtual void visit(const symbol *);
    virtual void visit(const number *);
    virtual void visit(const subscript_symbol *);
    virtual void visit(const subscript_number *);
    virtual void visit(const subscript_addition *);
};

class parse_tree_node{
public:
    parse_tree_node();
    
    void insert(parse_tree_node *);
    virtual void accept(parse_tree_node_visitor &)const;
    
    const parse_tree_node * get_prev()const;
    const parse_tree_node * get_next()const;
private:
    parse_tree_node * prev;
    parse_tree_node * next;
};

class label:public parse_tree_node{
public:
    static const processor::word ADDRESS_UNRESOLVED;
    lexer_token identifier;
    virtual void accept(parse_tree_node_visitor &)const;
};

class instruction:public parse_tree_node{
public:
    lexer_token operation;
    std::vector<parse_tree_node *> operands;
    
    virtual void accept(parse_tree_node_visitor &)const;
};

class number:public parse_tree_node{
public:
    lexer_token value;
    processor::word to_word()const;
    virtual void accept(parse_tree_node_visitor &)const;
};

class symbol:public parse_tree_node{
public:
    lexer_token value;
    
    virtual void accept(parse_tree_node_visitor &)const;
};

class subscript_symbol:public symbol{
public:
    virtual void accept(parse_tree_node_visitor &)const;
};

class subscript_number:public number{
public:
    virtual void accept(parse_tree_node_visitor &)const;
};

class subscript_addition:public parse_tree_node{
public:
    lexer_token number;
    lexer_token symbol;
    
    virtual void accept(parse_tree_node_visitor &)const;
};

parse_tree_node * parse(const char * begin, const char * end);

#endif
