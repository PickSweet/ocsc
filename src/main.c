#define PS_STRIP_PREFIX
#include "lexer.h"
#include "parser.h"
#define FILE_IMP
#include "../ps.h/ps_file.h"


const char* usage = "Usage: ./ocsc <file>.ocs";

int main(int argc, char **argv){
    char ***args = (char ***)(&(slice_t){argv,argc});
    if (_slice_size(args) < 2){
        log_print(LOG_ERROR, "%s\n", usage);
        return 1;
    }
    char *file = _slice_get(args, 1);
    char *dot = strrchr(file, '.');
    if (dot){
        if (strcmp(dot, ".ocs") != 0){
            log_print(LOG_ERROR, "File must have .ocs extension\n%s\n", usage);
            return 1;
        }
    }
    if (!file_exists(file)){
        log_print(LOG_ERROR, "File %s does not exist\n", file);
        return 1;
    }
    str_t content = file_read_all(file);
    lexer_t lexer = {
        .source = content,
        .position = 0
    };
    token_t **tokens = (token_t **)(&(list_t){0});
    token_t token = lexer_next(&lexer);
    while (token.tag != TOKEN_TAG_INVALID) {
        _list_append(tokens, token);
        log_print(LOG_INFO, "Token: tag=%s, start=%d, end=%d\n", token_tag_to_string(token.tag), token.span.start, token.span.end);
        log_print(LOG_INFO, "Token text: %.*s\n", token.span.end - token.span.start, content.items + token.span.start);
        token = lexer_next(&lexer);
    }
    if (token.span.start < content.size){
        log_print(LOG_ERROR, "Invalid Token:\n\t start=%d, end=%d\n\t text=\"%.*s\"\n", token.span.start, token.span.end, token.span.end - token.span.start, content.items + token.span.start);
        return 1;
    }
    log_print(LOG_INFO, "File content:\n%s\n", content.items);
    char **out_file_name = (char**)(&(str_t){0});
    _str_concatz(out_file_name, file, ".h");
    file_write(*out_file_name, &content,0);
    _list_free(&content.items);
    _list_free(out_file_name);
    return 0;
}