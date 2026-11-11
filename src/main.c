#define PS_STRIP_PREFIX
#define FILE_IMP
#include "../ps.h/ps_file.h"


const char* usage = "Usage: ./ocsc <file>";

int main(int argc, char **argv){
    char ***args = (char ***)(&(slice_t){argv,argc});
    if (_slice_size(args) < 2){
        log_print(LOG_ERROR, "%s\n", usage);
        return 1;
    }
    char *file = _slice_get(args, 1);
    if (!file_exists(file)){
        log_print(LOG_ERROR, "File %s does not exist\n", file);
        return 1;
    }
    str_t content = file_read_all(file);
    log_print(LOG_INFO, "File content:\n%s\n", content.items);
    char **out_file_name = (char**)(&(str_t){0});
    _str_concatz(out_file_name, file, ".h");
    file_write(*out_file_name, &content,0);
    _list_free(&content.items);
    _list_free(out_file_name);
    return 0;
}