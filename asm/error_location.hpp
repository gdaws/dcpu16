#ifndef DCPU16_ASM_ERROR_LOCATION_HPP
#define DCPU16_ASM_ERROR_LOCATION_HPP

#include "lexer.hpp"

class error_location{
public:
    error_location(int line, int column);
    error_location(const lexer_token &);
    int line()const;
    int column()const;
private:
    int _line;
    int _column;
};

#endif
