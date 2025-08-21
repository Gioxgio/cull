#include "dir.h"
#include <dirent.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

void open_dir(DIR **dir, char *path) {
    if ((*dir = opendir(path)) == NULL) {
        printf("Unable to open dir %s\n", path);
        perror("");
        exit(EXIT_FAILURE);
    }
}

bool is_file(struct dirent *de) { return de->d_type == DT_REG; }

bool is_user_dir(struct dirent *de) {
    return de->d_type == DT_DIR && strncmp(de->d_name, ".", NAME_MAX) &&
           strncmp(de->d_name, "..", NAME_MAX);
}

char *d_type_to_string(int t) {
    switch (t) {
    case DT_DIR:
        return "dir ";
    case DT_REG:
        return "File";
    }
    return "Ukwn";
}

void file_destroy(void *f) {
    struct file *file;
    file = (struct file *)f;
    free(file->name);
    free(file->path);
    free(file);
}
