#include "dir.h"
#include "sorted_list.h"
#include <limits.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

int file_sort(void *, void *);
int string_sort(void *, void *);

void load_dir_content(SortedList, char *);
void find_duplicates(SortedList, SortedList);

int main(int argc, char **argv) {

    SortedList l1, l2;

    if (argc != 3) {
        printf("You need to pass two directories!\n");
        exit(EXIT_FAILURE);
    }

    l1 = list_create(file_sort);
    l2 = list_create(file_sort);

    printf("Loading %s content... ", argv[1]);
    load_dir_content(l1, argv[1]);
    printf("Done\n");
    printf("Loading %s content... ", argv[2]);
    load_dir_content(l2, argv[2]);
    printf("Done\n");

    printf("\nComparing files...\n\n");
    find_duplicates(l1, l2);
    printf("\nDone\n");

    list_destroy(l1, file_destroy);
    list_destroy(l2, file_destroy);

    printf("Done!\n");

    return 0;
}

int file_sort(void *a, void *b) {
    struct file *f1, *f2;

    f1 = (struct file *)a;
    f2 = (struct file *)b;

    return strncmp(f1->name, f2->name, NAME_MAX);
}

int string_sort(void *a, void *b) {
    char *s1, *s2;

    s1 = (char *)a;
    s2 = (char *)b;

    return strncmp(s1, s2, NAME_MAX);
}

void load_dir_content(SortedList l, char *path) {
    char *current_path, *new_path;
    SortedList paths;
    DIR *dir;
    struct dirent *de;
    struct file *f;

    paths = list_create(string_sort);
    list_push(paths, strndup(path, strnlen(path, NAME_MAX)));

    while ((current_path = list_pop(paths)) != NULL) {
        open_dir(&dir, current_path);
        while ((de = readdir(dir)) != NULL) {
            if (is_file(de)) {
                f = malloc(sizeof(*f));
                f->name = strndup(de->d_name, NAME_MAX);
                f->path = strndup(current_path, NAME_MAX);
                list_push(l, f);
            } else if (is_user_dir(de)) {
                new_path = malloc(strnlen(current_path, NAME_MAX) +
                                  strnlen(de->d_name, NAME_MAX) + 2);
                sprintf(new_path, "%s/%s", current_path, de->d_name);
                list_push(paths, new_path);
            }
        }
        free(current_path);
        closedir(dir);
    }

    list_destroy(paths, free);
}

void find_duplicates(SortedList l_list, SortedList r_list) {
    char *l_name, *r_name;
    struct file *l_item, *r_item;
    int res;

    r_item = list_pop(r_list);
    l_item = list_pop(l_list);

    while (l_item != NULL && r_item != NULL) {
        r_name = r_item->name;
        l_name = l_item->name;

        res = strncmp(l_name, r_name, NAME_MAX);

        if (res < 0) {
            file_destroy(l_item);
            l_item = list_pop(l_list);
        } else if (res == 0) {
            printf("DUPL:\t%s\n", r_name);
            file_destroy(l_item);
            file_destroy(r_item);
            r_item = list_pop(r_list);
            l_item = list_pop(l_list);
        } else { // result > 0
            // printf("NEW :\t%s\n", r_name);
            file_destroy(r_item);
            r_item = list_pop(r_list);
        }
    }

    while ((r_item = list_pop(r_list)) != NULL) {
        file_destroy(r_item);
        // r_name = r_item->name;
        // printf("NEW :\t%s\n", r_name);
    }
}
