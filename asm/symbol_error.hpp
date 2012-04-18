#ifndef DCPU16_ASM_SYMBOL_ERROR_HPP
#define DCPU16_ASM_SYMBOL_ERROR_HPP

#include "error_location.hpp"
#include <stdexcept>

class symbol_error:public std::runtime_error{
public:
    symbol_error(const std::string & what, const error_location & where);
    const error_location & where()const;
private:
    error_location _where;
};

#endif
