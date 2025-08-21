#ifndef DIR_H

#define DIR_F
#define _DEFAULT_SOURCE
#include <dirent.h>
#include <stdbool.h>

struct file {
    char *name;
    char *path;
};

void open_dir(DIR **, char *);
bool is_file(struct dirent *);
bool is_user_dir(struct dirent *);
char *d_type_to_string(int);
void file_destroy(void *);
#endif
