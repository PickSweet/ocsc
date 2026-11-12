#ifndef _H_LEXER_H_
#define _H_LEXER_H_

#define PS_STRIP_PREFIX
#include "../ps.h/ps_collections.h"

typedef struct {
    ps_str_t source;
    half_t position;
} lexer_t;

typedef struct {
    half_t start;
    half_t end;
} span_t;

typedef enum {
    TOKEN_TAG_INVALID,
    TOKEN_TAG_KEYWORD_USING,
    TOKEN_TAG_IDENTIFIER,
    TOKEN_TAG_WHITESPACE,
    TOKEN_TAG_STRING,
    TOKEN_TAG_SINGLE_LINE_COMMENT,
    TOKEN_TAG_MULTI_LINE_COMMENT,
    TOKEN_TAG_START_STRUCT,
    TOKEN_TAG_LBRACKET,
    TOKEN_TAG_RBRACKET,
    TOKEN_TAG_LBRACE,
    TOKEN_TAG_RBRACE,
    TOKEN_TAG_SEMICOLON,
    TOKEN_TAG_EQUAL,
} token_tag_e;

typedef struct {
    token_tag_e tag;
    span_t span;
} token_t;

token_t lexer_next(lexer_t *lexer);
const char *token_tag_to_string(token_tag_e tag);

#endif // _H_LEXER_H_