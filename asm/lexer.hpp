#ifndef ASM_LEXER_HPP
#define ASM_LEXER_HPP

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

struct lexer_token:public lexer_string{
    
    LEXER_TOKEN_TYPE type;
    
    unsigned short line;
    
    lexer_token();
    lexer_token(const char *, const char *);
};

bool scan(const char *, const char *, lexer_token &);

class lexer:public lexer_token{
public:
    lexer(const char * begin, const char * end);
    
    bool next();
    
    bool at_end()const;
    
    unsigned short column()const;
    unsigned short column(const lexer_token &)const;
private:
    const char * source_begin;
    const char * source_end;
};

#endif
