#ifndef DCPU16_ASM_PARSE_ERROR_HPP
#define DCPU16_ASM_PARSE_ERROR_HPP

#include "lexer.hpp"
#include <stdexcept>

class parse_error:public std::runtime_error{
public:
    parse_error(const std::string & what, const lexer & where);
    const lexer & where()const;
    static void throw_unexpected(const lexer & where);
private:
    const lexer _where;
};

#endif
