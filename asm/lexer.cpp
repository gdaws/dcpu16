#include "lexer.hpp"
#include "parse_error.hpp"
#include <cstring>
#include <algorithm>

static const LEXER_TOKEN_TYPE token_entry_symbol[] = {
    ERROR,ERROR,ERROR,ERROR,ERROR,ERROR,ERROR,ERROR,
    ERROR,WHITESPACE,NEWLINE,ERROR,ERROR,NEWLINE,ERROR,ERROR,
    ERROR,ERROR,ERROR,ERROR,ERROR,ERROR,ERROR,ERROR,
    ERROR,ERROR,ERROR,ERROR,ERROR,ERROR,ERROR,ERROR,
    WHITESPACE,ERROR,STRING,ERROR,ERROR,ERROR,ERROR,ERROR,
    ERROR,ERROR,ERROR,PLUS,COMMA,ERROR,ERROR,ERROR,
    NUMBER,NUMBER,NUMBER,NUMBER,NUMBER,NUMBER,NUMBER,NUMBER,
    NUMBER,NUMBER,LABEL,COMMENT,ERROR,ERROR,ERROR,ERROR,
    ERROR,WORD,WORD,WORD,WORD,WORD,WORD,WORD,
    WORD,WORD,WORD,WORD,WORD,WORD,WORD,WORD,
    WORD,WORD,WORD,WORD,WORD,WORD,WORD,WORD,
    WORD,WORD,WORD,OPEN_SUBSCRIPT,ERROR,CLOSE_SUBSCRIPT,ERROR,WORD,
    ERROR,WORD,WORD,WORD,WORD,WORD,WORD,WORD,
    WORD,WORD,WORD,WORD,WORD,WORD,WORD,WORD,
    WORD,WORD,WORD,WORD,WORD,WORD,WORD,WORD,
    WORD,WORD,WORD,ERROR,ERROR,ERROR,ERROR
};

static const bool word_symbols[] = {
    0,0,0,0,0,0,0,0,
    0,0,0,0,0,0,0,0,
    0,0,0,0,0,0,0,0,
    0,0,0,0,0,0,0,0,
    0,0,0,0,0,0,0,0,
    0,0,0,0,0,0,0,0,
    1,1,1,1,1,1,1,1,
    1,1,0,0,0,0,0,0,
    0,1,1,1,1,1,1,1,
    1,1,1,1,1,1,1,1,
    1,1,1,1,1,1,1,1,
    1,1,1,0,0,0,0,1,
    0,1,1,1,1,1,1,1,
    1,1,1,1,1,1,1,1,
    1,1,1,1,1,1,1,1,
    1,1,1,0,0,0,0
};

static const bool number_symbols[] = {
    0,0,0,0,0,0,0,0,
    0,0,0,0,0,0,0,0,
    0,0,0,0,0,0,0,0,
    0,0,0,0,0,0,0,0,
    0,0,0,0,0,0,0,0,
    0,0,0,0,0,0,0,0,
    1,1,1,1,1,1,1,1,
    1,1,0,0,0,0,0,0,
    0,1,1,1,1,1,1,0,
    0,0,0,0,0,0,0,0,
    0,0,0,0,0,0,0,0,
    0,0,0,0,0,0,0,0,
    0,1,1,1,1,1,1,0,
    0,0,0,0,0,0,0,0,
    0,0,0,0,0,0,0,0,
    1,0,0,0,0,0,0
};

static const bool string_symbols[] = {
    0,0,0,0,0,0,0,0,
    0,0,0,0,0,0,0,0,
    0,0,0,0,0,0,0,0,
    0,0,0,0,0,0,0,0,
    1,1,0,1,1,1,1,1,
    1,1,1,1,1,1,1,1,
    1,1,1,1,1,1,1,1,
    1,1,1,1,1,1,1,1,
    1,1,1,1,1,1,1,1,
    1,1,1,1,1,1,1,1,
    1,1,1,1,1,1,1,1,
    1,1,1,1,1,1,1,1,
    1,1,1,1,1,1,1,1,
    1,1,1,1,1,1,1,1,
    1,1,1,1,1,1,1,1,
    1,1,1,1,1,1,1
};

lexer_string::lexer_string(){
    
}

lexer_string::lexer_string(const char * begin)
:begin(begin), end(begin + strlen(begin)){

}

lexer_string::lexer_string(const char * begin, const char * end)
:begin(begin), end(end){
    
}

static int compare_strings(const lexer_string & a, const lexer_string & b){
    return strncasecmp(a.begin, b.begin, std::min(a.end - a.begin, b.end - b.begin));
}

bool lexer_string::operator<(const lexer_string & compare_with)const{
    return compare_strings(*this, compare_with) < 0;
}

bool lexer_string::operator==(const lexer_string & compare_with)const{
    return compare_strings(*this, compare_with) == 0;
}

lexer_token::lexer_token()
:type(ERROR), line(1), column(1){
    
}

lexer_token::lexer_token(const char * begin, const char * end)
:lexer_string(begin, end), type(ERROR), line(1){
    
}

bool scan(const char * begin, const char * end, lexer_token & token){
    
    if(token.type != NEWLINE){
        token.column = token.column + (token.end - token.begin);
    }
    else{
        token.column = 1;
    }
    
    token.type = token_entry_symbol[static_cast<int>(*begin)];
    token.begin = begin;
    
    const char * cur = begin + 1;
    
    switch(token.type){
        
        case WHITESPACE:
            for(; cur != end && token_entry_symbol[static_cast<int>(*cur)] == WHITESPACE; cur++);
            break;
        
        case NEWLINE:
            
            token.line++;
            
            if(*begin == '\r' && cur != end && *cur == '\n'){
                cur++;
            }
            
            break;
        
        case WORD:
            for(; cur != end && word_symbols[static_cast<int>(*cur)]; cur++);
            break;
        
        case NUMBER:
            for(; cur != end && number_symbols[static_cast<int>(*cur)]; cur++);
            break;
        
        case COMMENT:
            for(; cur != end && (*cur != '\r' && *cur != '\n'); cur++);
            break;
        
        case STRING:
            
            for(; cur != end && string_symbols[static_cast<int>(*cur)]; cur++);
            
            if(cur == end || token_entry_symbol[static_cast<int>(*cur)] != STRING){
                throw parse_error("expected '\"' before the end of the line", error_location(token));
            }
            
            cur++;
            
            break;
        
        default:;
    }
    
    token.end = cur;
    
    return true;
}

lexer::lexer(const char * source_begin, const char * source_end)
:lexer_token(source_begin, source_begin), source_begin(source_begin), source_end(source_end){
    
}

bool lexer::next(){
    
    if(at_end()){
        return false;
    }
    
    return scan(end, source_end, *this);
}

bool lexer::at_end()const{
    return end == source_end;
}
