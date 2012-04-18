#include "parse_error.hpp"
#include <sstream>

parse_error::parse_error(const std::string & what, const error_location & where)
:std::runtime_error(what), _where(where){
    
}

const error_location & parse_error::where()const{
    return _where;
}

void parse_error::throw_unexpected(const lexer & where){
    
    std::stringstream message;
    message << "unexpected ";
    
    char symbol = *where.begin;
    
    if(symbol >= 32 && symbol <= 126){
        message << "'" << symbol << "'";
    }
    else{
        
        const char * code[] = {
            "NULL", "SOH", "STX", "ETX", "EOT", 
            "ENQ", "ACK", "BEL", "BS", "TAB", 
            "LF", "VT", "FF", "CR", "SO", "SI",
            "DLE", "DC1", "DC2", "DC4", "NAK", "SYN",
            "ETB", "CAN", "EM", "SUB", "ESC", "FS", 
            "GS", "RS", "US"
        };
        
        if(symbol == 0x7F){
            message << "DEL character";
        }
        else{
            message << code[static_cast<int>(symbol)] << " character";   
        }
    }
    
    throw parse_error(message.str(), error_location(where.line, where.column));
}
