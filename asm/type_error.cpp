#include "type_error.hpp"

type_error::type_error(const std::string & what, const error_location & where)
:std::runtime_error(what), _where(where){
    
}

const error_location & type_error::where()const{
    return _where;
}
