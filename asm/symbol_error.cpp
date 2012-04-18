#include "symbol_error.hpp"

symbol_error::symbol_error(const std::string & what, const error_location & where)
:std::runtime_error(what), _where(where){
    
}

const error_location & symbol_error::where()const{
    return _where;
}
