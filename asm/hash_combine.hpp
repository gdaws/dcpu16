#ifndef DCPU16_ASM_HASH_COMBINE_HPP
#define DCPU16_ASM_HASH_COMBINE_HPP

#include <functional>

/* Copied from http://stackoverflow.com/questions/2590677/how-do-i-combine-hash-values-in-c0x */
template <class T>
inline void hash_combine(std::size_t& seed, const T& v){
    std::hash<T> hasher;
    seed ^= hasher(v) + 0x9e3779b9 + (seed<<6) + (seed>>2);
}

#endif
