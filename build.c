#define PS_STRIP_PREFIX
#define BUILD_IMP
#include "./ps.h/ps_build.h"

#ifdef _WIN32
#define EXE_SUFFIX ".exe"
#else
#define EXE_SUFFIX ""
#endif

int build(builder_t *builder){
    _build_rebuild_yourself(builder->args);
    idx_t main_group = _build_append_group(builder);
    idx_t object_group = _build_append_child_group(builder, main_group);
    path_list_t c_files = dir_find_files("./src/","*.c",1);
    char ***o = (char ***)(&(list_t){0});
    _list_foreach(&c_files.items, file){
        char **file_path = (char **)file;
        char *file_path_cpy = strdup(*file_path);
        if (!file_path_cpy){
            log_print(LOG_ERROR, "Failed to duplicate file path %s", *file_path);
            return 1;
        }
        char *dot = strrchr(file_path_cpy, '.');
        if (dot)
            *(dot + 1) = 'o';
        _list_append(o, file_path_cpy);
        char **sub_cmd = (char**)(&(str_t){0});
        str_append_fmt(sub_cmd, "%s -c %s -o %s", BUILD_CC, *file_path, file_path_cpy);
        _build_append_child_shell_cmd(builder, object_group, *sub_cmd);
        _build_free_after_run(builder, *sub_cmd);
        _build_free_after_run(builder, file_path_cpy);
        _build_free_after_run(builder, *file_path);
    }

    char ***cmd = (char ***)(&(list_t){0});
    _list_append(cmd, BUILD_CC);
    _list_foreach(o, file_path){
        _list_append(cmd, *file_path);
    }
    _list_append(cmd, "-o", "ocsc" EXE_SUFFIX);

    _build_append_child_shell_cmd_list(builder, main_group, cmd);
    _build_append_child_shell_cmd(builder, main_group, "ocsc" EXE_SUFFIX, "./test.ocs");
    _build_free_after_run(builder, c_files.items);
    _build_free_after_run(builder, *o);
    _build_free_after_run(builder, *cmd);
    return 0;
}