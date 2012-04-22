#ifndef ASM_LEXER_HPP
#define ASM_LEXER_HPP

#include "hash_combine.hpp"
#include <string>
#include <functional>

enum LEXER_TOKEN_TYPE{
    ERROR = 0,
    WHITESPACE,
    NEWLINE,
    WORD,
    NUMBER,
    COMMENT,
    LABEL,
    COMMA,
    OPEN_SUBSCRIPT,
    CLOSE_SUBSCRIPT,
    PLUS
};

struct lexer_string{
    
    const char * begin;
    const char * end;
    
    lexer_string();
    lexer_string(const char *);
    lexer_string(const char *, const char *);
    
    bool operator<(const lexer_string &)const;
    bool operator==(const lexer_string &)const;
};

namespace std{
template<>
class hash<lexer_string>{
public:
    std::size_t operator()(const lexer_string & string)const{
        std::size_t hash_value;
        for(const char * cur = string.begin; cur != string.end; cur++){
            char c = (*cur >= 'A' && *cur <= 'Z' ? 'a' + (*cur - 'A'): *cur);
            if(cur == string.begin){
                hash_value = std::hash<char>()(c);
            }
            else{
                hash_value = hash_combine(hash_value, c);
            }
        }
        return hash_value;
    }
};
} // namespace std

struct lexer_token:public lexer_string{
    
    LEXER_TOKEN_TYPE type;
    
    unsigned short line;
    unsigned short column;
    
    lexer_token();
    lexer_token(const char *, const char *);
};

bool scan(const char *, const char *, lexer_token &);

class lexer:public lexer_token{
public:
    lexer(const char * begin, const char * end);
    
    bool next();
    
    bool at_end()const;
private:
    const char * source_begin;
    const char * source_end;
};

#endif
