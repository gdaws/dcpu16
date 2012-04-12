#include "processor_exception.hpp"

processor_exception::processor_exception(const std::string & what)
:std::runtime_error(what){
    
}
