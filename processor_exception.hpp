#ifndef PROCESSOR_EXCEPTION_HPP
#define PROCESSOR_EXCEPTION_HPP

#include <stdexcept>

class processor_exception:public std::runtime_error{
public:
    processor_exception(const std::string & what);
};

#endif
