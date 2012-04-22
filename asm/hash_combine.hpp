#ifndef DCPU16_ASM_HASH_COMBINE_HPP
#define DCPU16_ASM_HASH_COMBINE_HPP

#include <functional>

/* 
    http://www.boost.org/doc/libs/1_33_1/doc/html/hash_combine.html
*/
template <class T>
inline std::size_t hash_combine(const std::size_t & seed, const T & value){
    return seed ^ (std::hash<T>()(value) + 0x9e3779b9 + (seed<<6) + (seed>>2));
}

#endif
