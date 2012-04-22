#ifndef DCPU16_TYPE_ERROR_HPP
#define DCPU16_TYPE_ERROR_HPP

#include <stdexcept>
#include "error_location.hpp"

class type_error:public std::runtime_error{
public:
    type_error(const std::string & what, const error_location & where);
    const error_location & where()const;
private:
    error_location _where;
};

#endif
