#ifndef DCPU16_ASM_PARSE_ERROR_HPP
#define DCPU16_ASM_PARSE_ERROR_HPP

#include "error_location.hpp"
#include <stdexcept>

class parse_error:public std::runtime_error{
public:
    parse_error(const std::string & what, const error_location &);
    const error_location & where()const;
    static void throw_unexpected(const lexer & where);
private:
    error_location _where;
};

#endif
