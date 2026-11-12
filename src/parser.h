#ifndef _H_PARSER_H_
#define _H_PARSER_H_

#include "lexer.h"


typedef struct {
    // preamble is anything before the name
    span_t preamble;
    // postamble is anything after the name but before the = sign or semi colon determined by braces []
    span_t postamble;
    // value is anything after the = sign and before the semi colon
    span_t value;
    // name is the identifier of the field
    half_t name;
} field_t;

typedef struct {
    field_t* items;
    half_t size;
    half_t capacity;
} field_list_t;

typedef struct {
    token_t* items;
    half_t size;
} token_slice_t;

typedef struct {
    token_slice_t tokens;
    field_list_t fields;
} parser_t;


int parse(parser_t *parser);
int parse_keyword(parser_t *parser);
int parse_field(parser_t *parser);

#endif