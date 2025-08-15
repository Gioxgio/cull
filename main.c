#define _DEFAULT_SOURCE

#include <dirent.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define MAX_NAME_SIZE 256
#define MAX_PATH_SIZE 1024
struct file {
    char *name;
    char *path;
};
void file_print(void *);
int file_sort(void *, void *);
void string_print(void *);
int string_sort(void *, void *);

/* LIST */
typedef struct list *List;
typedef void *Item;
struct list {
    struct node *top;
    int (*cmp)(Item, Item);
};
struct node {
    Item item;
    struct node *next;
};

List list_create(int(Item, Item));
void list_push(List, Item);
Item list_pop(List);
void list_each(List, void(Item));
void list_destroy(List);
/* DIR */
void open_dir(DIR **, char *);
void load_dir_content(List, char *);
bool is_user_dir(struct dirent *);
char *d_type_to_string(int);

// TODO
// Replace list with SortedList
// - [ ] Add comparation
// - [ ] Fix TODO
// - [ ] Split code into files + create makefile
// - [ ] Add pop method
int main(int argc, char **argv) {

    DIR *d1, *d2;
    List l1, l2;

    if (argc != 3) {
        printf("You need to pass two directories!\n");
        exit(EXIT_FAILURE);
    }

    d1 = NULL;
    d2 = NULL;

    l1 = list_create(file_sort);
    l2 = list_create(file_sort);

    load_dir_content(l1, argv[1]);
    load_dir_content(l2, argv[2]);

    printf("Files in folder folder %s\n", argv[1]);
    list_each(l1, file_print);
    printf("Files in folder folder %s\n", argv[2]);
    list_each(l2, file_print);

    closedir(d1);
    closedir(d2);

    list_destroy(l1);
    list_destroy(l2);

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

    printf("name1: %s, name2: %s, result: %d\n", f1->name, f2->name,
           strncmp(f1->name, f2->name, MAX_NAME_SIZE));

    return strncmp(f1->name, f2->name, MAX_NAME_SIZE);
}

int string_sort(void *a, void *b) {
    char *s1, *s2;

    s1 = (char *)a;
    s2 = (char *)b;

    return strncmp(s1, s2, MAX_NAME_SIZE);
}

/* DIR */
void open_dir(DIR **dir, char *path) {
    if ((*dir = opendir(path)) == NULL) {
        printf("Unable to open dir %s\n", path);
        perror("");
        exit(EXIT_FAILURE);
    }
}

void load_dir_content(List l, char *path) {
    char *current_path, *new_path;
    List paths;
    DIR *dir;
    struct dirent *de;
    struct file *f;

    paths = list_create(string_sort);
    list_push(paths, strndup(path, strnlen(path, MAX_NAME_SIZE)));

    while ((current_path = list_pop(paths)) != NULL) {
        open_dir(&dir, current_path);
        while ((de = readdir(dir)) != NULL) {
            if (de->d_type == DT_REG) {
                f = malloc(sizeof(*de));
                f->name = strndup(de->d_name, MAX_NAME_SIZE);
                f->path = strndup(current_path, MAX_NAME_SIZE);
                list_push(l, f);
            } else if (de->d_type == DT_DIR &&
                       strncmp(de->d_name, ".", MAX_NAME_SIZE) &&
                       strncmp(de->d_name, "..", MAX_NAME_SIZE)) {

                new_path = malloc(strnlen(current_path, MAX_NAME_SIZE) +
                                  strnlen(de->d_name, MAX_NAME_SIZE) + 2);
                sprintf(new_path, "%s/%s", current_path, de->d_name);
                list_push(paths, new_path);
            }
        }
    }

    list_destroy(paths);
}

void compare_lists(List left, List right) {
    char l_filename, r_filename;
    struct file *l_item, *r_item;

    if ((l_item = list_pop(left)) == NULL &&
        (r_item = list_pop(right)) == NULL) {
        printf("One of the folders is empty\n");
        return;
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

/* list */
List list_create(int cmp(Item, Item)) {
    List l;
    // TODO handle failure
    l = malloc(sizeof(*l));
    l->cmp = cmp;
    l->top = NULL;
    return l;
}

void list_push(List l, Item i) {
    struct node *new, *prev, *curr;
    // TODO handle failure
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

Item list_pop(List l) {
    struct node *top;

    if (l == NULL) {
        return NULL;
    }

    top = l->top;
    if (l->top == NULL) {
        return NULL;
    }

    l->top = top->next;

    return top->item;
}

void list_each(List l, void consumer(Item)) {
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

// TODO Add destroy function for file and pass it to this function
void list_destroy(List l) {
    struct node *next, *top;

    if (l == NULL) {
        return;
    }

    top = l->top;
    while (top) {
        next = top->next;
        // TODO handle failure
        free(top);
        top = next;
    }

    // TODO handle failure
    free(l);
}
