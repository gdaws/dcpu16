#include "error_location.hpp"

error_location::error_location(int line, int column)
:_line(line), _column(column){
    
}

error_location::error_location(const lexer_token & token)
:_line(token.line), _column(token.column){
    
}

int error_location::line()const{
    return _line;
}

int error_location::column()const{
    return _column;
}
