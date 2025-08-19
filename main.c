#define _DEFAULT_SOURCE

#include <dirent.h>
#include <limits.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

struct file {
    char *name;
    char *path;
};
void file_print(void *);
int file_sort(void *, void *);
void string_print(void *);
int string_sort(void *, void *);

/* LIST */
typedef struct list *SortedList;
typedef void *Item;
struct list {
    struct node *top;
    int (*cmp)(Item, Item);
};
struct node {
    Item item;
    struct node *next;
};

SortedList list_create(int(Item, Item));
void list_push(SortedList, Item);
Item list_pop(SortedList);
void list_each(SortedList, void(Item));
void list_destroy(SortedList, void(Item));
/* DIR */
void open_dir(DIR **, char *);
void load_dir_content(SortedList, char *);
bool is_user_dir(struct dirent *);
char *d_type_to_string(int);
void file_destroy(Item);

void find_duplicates(SortedList, SortedList);

// TODO
// - [ ] Split code into files + create makefile
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

void file_print(void *f) {
    struct file *file = (struct file *)f;
    printf("File name: %s File path: %s\n", file->name, file->path);
}

void string_print(void *s) { printf(" %s\n", (char *)s); }

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

/* DIR */
void open_dir(DIR **dir, char *path) {
    if ((*dir = opendir(path)) == NULL) {
        printf("Unable to open dir %s\n", path);
        perror("");
        exit(EXIT_FAILURE);
    }
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
            if (de->d_type == DT_REG) {
                f = malloc(sizeof(*f));
                f->name = strndup(de->d_name, NAME_MAX);
                f->path = strndup(current_path, NAME_MAX);
                list_push(l, f);
            } else if (de->d_type == DT_DIR &&
                       strncmp(de->d_name, ".", NAME_MAX) &&
                       strncmp(de->d_name, "..", NAME_MAX)) {

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
        // r_name = r_item->name;
        // printf("NEW :\t%s\n", r_name);
    }
}

bool is_user_dir(struct dirent *de) { return DT_DIR == de->d_type; }

char *d_type_to_string(int t) {
    switch (t) {
    case DT_DIR:
        return "Dir ";
    case DT_REG:
        return "File";
    }
    return "Ukwn";
}

void file_destroy(Item f) {
    struct file *file;
    file = (struct file *)f;
    free(file->name);
    free(file->path);
    free(file);
}

/* list */
SortedList list_create(int cmp(Item, Item)) {
    SortedList l;
    l = malloc(sizeof(*l));
    l->cmp = cmp;
    l->top = NULL;
    return l;
}

void list_push(SortedList l, Item i) {
    struct node *new, *prev, *curr;

    new = malloc(sizeof(*new));
    new->item = i;

    curr = l->top;
    prev = NULL;

    while (curr != NULL && l->cmp(i, curr->item) > 0) {
        prev = curr;
        curr = curr->next;
    }
    if (prev == NULL) {
        l->top = new;
        new->next = curr;
    } else {
        prev->next = new;
        new->next = curr;
    }
}

Item list_pop(SortedList l) {
    struct node *top;
    Item i;

    if (l == NULL) {
        return NULL;
    }

    top = l->top;
    if (l->top == NULL) {
        return NULL;
    }

    l->top = top->next;
    i = top->item;
    free(top);

    return i;
}

void list_each(SortedList l, void consumer(Item)) {
    struct node *top;

    if (l == NULL) {
        return;
    }

    top = l->top;
    while (top) {
        consumer(top->item);
        top = top->next;
    }
}

void list_destroy(SortedList l, void item_destroyer(Item)) {
    struct node *next, *top;

    if (l == NULL) {
        return;
    }

    top = l->top;
    while (top) {
        next = top->next;
        item_destroyer(top->item);
        free(top);
        top = next;
    }

    free(l);
}
