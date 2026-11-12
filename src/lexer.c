#define PS_DEF static
#define PS_CORE_IMP
#include "lexer.h"

typedef enum {
    LEXER_STATE_START,
    LEXER_STATE_WHITESPACE,
    LEXER_STATE_STRING,
    LEXER_STATE_SINGLE_LINE_COMMENT,
    LEXER_STATE_MULTI_LINE_COMMENT,
    LEXER_STATE_KEYWORD,
    LEXER_STATE_IDENTIFIER,
} lexer_state_e;

#define KEYWORDS_SIZE 1
static const char *KEYWORDS[KEYWORDS_SIZE] = {
    "#using",
};

static const token_tag_e KEYWORD_TAGS[KEYWORDS_SIZE] = {
    TOKEN_TAG_KEYWORD_USING,
};

static const char *KEYWORD_NAMES[KEYWORDS_SIZE] = {
    "KEYWORD_USING",
};

static const char WHITESPACE[] = " \t\n\r";

static const char *TOKEN_TAG_NAMES[] = {
    "INVALID",
    "KEYWORD",
    "IDENTIFIER",
    "WHITESPACE",
    "STRING",
    "SINGLE_LINE_COMMENT",
    "MULTI_LINE_COMMENT",
    "START_STRUCT",
    "LBRACKET",
    "RBRACKET",
    "LBRACE",
    "RBRACE",
    "SEMICOLON",
    "EQUAL",
};

const char *token_tag_to_string(token_tag_e tag) {
    half_t i = KEYWORDS_SIZE - 1;
    do {
        if (KEYWORD_TAGS[i] == tag) {
            return KEYWORD_NAMES[i];
        }
    } while (i--);
    return TOKEN_TAG_NAMES[tag];
}

token_t lexer_next(lexer_t *lexer) {
    token_t token = {
        TOKEN_TAG_INVALID,
        {lexer->position, lexer->position}
    };
    lexer_state_e state = LEXER_STATE_START;
    state_l:
    if (lexer->position > lexer->source.size && lexer->source.items[lexer->source.size] != '\0') {
        return token;
    }
    switch (state){
        case LEXER_STATE_START: {
            const char c = lexer->source.items[lexer->position];
            if (c == '\0') {
                break;
            }else if (strchr(WHITESPACE, c)) {
                token.tag = TOKEN_TAG_WHITESPACE;
                state = LEXER_STATE_WHITESPACE;
                goto state_l;
            } else if (c == '.'){
                const char next_c = lexer->source.items[lexer->position + 1];
                if (next_c == '{'){
                    token.tag = TOKEN_TAG_START_STRUCT;
                    lexer->position += 2;
                    token.span.end += 2;
                }
            } else if (c == '{'){
                token.tag = TOKEN_TAG_LBRACE;
                lexer->position++;
                token.span.end++;
            } else if (c == '}'){
                token.tag = TOKEN_TAG_RBRACE;
                lexer->position++;
                token.span.end++;
            } else if (c == '['){
                token.tag = TOKEN_TAG_LBRACKET;
                lexer->position++;
                token.span.end++;
            } else if (c == ']'){
                token.tag = TOKEN_TAG_RBRACKET;
                lexer->position++;
                token.span.end++;
            } else if (c == ';'){
                token.tag = TOKEN_TAG_SEMICOLON;
                lexer->position++;
                token.span.end++;
            } else if (c == '='){
                token.tag = TOKEN_TAG_EQUAL;
                lexer->position++;
                token.span.end++;
            } else if (c == '"'){
                token.tag = TOKEN_TAG_STRING;
                state = LEXER_STATE_STRING;
                lexer->position++;
                token.span.end++;
                goto state_l;
            } else if (c == '#'){
                token.tag = TOKEN_TAG_IDENTIFIER;
                state = LEXER_STATE_KEYWORD;
                lexer->position++;
                token.span.end++;
                goto state_l;
            } else if (c == '/'){
                const char next_c = lexer->source.items[lexer->position + 1];
                if (next_c == '/'){
                    token.tag = TOKEN_TAG_SINGLE_LINE_COMMENT;
                    state = LEXER_STATE_SINGLE_LINE_COMMENT;
                    lexer->position += 2;
                    token.span.end += 2;
                    goto state_l;
                } else if (next_c == '*'){
                    token.tag = TOKEN_TAG_MULTI_LINE_COMMENT;
                    state = LEXER_STATE_MULTI_LINE_COMMENT;
                    lexer->position += 2;
                    token.span.end += 2;
                    goto state_l;
                }
            } else {
                token.tag = TOKEN_TAG_IDENTIFIER;
                state = LEXER_STATE_IDENTIFIER;
                goto state_l;
            }
            break;
        }
        case LEXER_STATE_WHITESPACE:{
            const char c = lexer->source.items[lexer->position];
            if (strchr(WHITESPACE, c)) {
                lexer->position++;
                token.span.end++;
                goto state_l;
            }
            break;
        }
        case LEXER_STATE_STRING:{
            const char c = lexer->source.items[lexer->position];
            if (c == '\0') {
                token.tag = TOKEN_TAG_INVALID;
                break;
            }
            if (c == '"') {
                lexer->position++;
                token.span.end++;
            } else {
                lexer->position++;
                token.span.end++;
                goto state_l;
            }
            break;
        }
        case LEXER_STATE_SINGLE_LINE_COMMENT:{
            const char c = lexer->source.items[lexer->position];
            if (c == '\n' || c == '\0') {
                break;
            }
            lexer->position++;
            token.span.end++;
            goto state_l;
        }
        case LEXER_STATE_MULTI_LINE_COMMENT:{
            const char c = lexer->source.items[lexer->position];
            if (c == '*') {
                const char next_c = lexer->source.items[lexer->position + 1];
                if (next_c == '/') {
                    lexer->position += 2;
                    token.span.end += 2;
                    break;
                }
            } else if (c == '\0') {
                token.tag = TOKEN_TAG_INVALID;
                break;
            }
            lexer->position++;
            token.span.end++;
            goto state_l;
        }
        case LEXER_STATE_KEYWORD:{
            const char c = lexer->source.items[lexer->position];
            if (strchr(WHITESPACE, c) || c == '\0' || c == ';') {
                str_view_t keyword = (str_view_t){lexer->source.items + token.span.start, token.span.end - token.span.start};
                int i = KEYWORDS_SIZE - 1;
                do {
                    if (mem_eql(keyword.items, KEYWORDS[i], keyword.size)){
                        token.tag = KEYWORD_TAGS[i];
                        break;
                    }
                }while (i--);
            } else {
                lexer->position++;
                token.span.end++;
                goto state_l;
            }
            break;
        }
        case LEXER_STATE_IDENTIFIER:{
            const char c = lexer->source.items[lexer->position];
            if (!strchr(WHITESPACE, c) && c != '\0' && !strchr("[]{};=/*\\", c)) {
                lexer->position++;
                token.span.end++;
                goto state_l;
            }
            break;
        }
    }

    return token;
}