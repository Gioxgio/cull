#define _DEFAULT_SOURCE

#include <dirent.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/dir.h>

#define MAX_NAME_SIZE 512
#define MAX_PATH_SIZE 1024
struct file {
    char name[MAX_NAME_SIZE];
    char path[MAX_PATH_SIZE];
};
void file_print(void *);

/* DIR */
void open_dir(DIR **, char *);
void print_dir_content(DIR *, char *);
bool is_user_dir(struct dirent *);
char *d_type_to_string(int);
/* QUEUE */
typedef struct queue *Queue;
typedef void *Item;
struct queue {
    struct node *top;
};
struct node {
    Item item;
    struct node *next;
};

Queue queue_create();
void queue_push(Queue, Item);
void queue_each(Queue, void(Item));
void queue_destroy(Queue);

// TODO
// - [ ] Populate queue
// - [ ] Fix TODO
// - [ ] Split code into files + create makefile
// - [ ] Add pop method
int main(int argc, char **argv) {

    DIR *d1, *d2;
    Queue q;

    if (argc != 3) {
        printf("You need to pass two directories!\n");
        exit(EXIT_FAILURE);
    }

    d1 = NULL;
    d2 = NULL;

    q = queue_create();

    queue_push(q, &(struct file){"Giorgio", "Giacomo"});
    queue_each(q, file_print);
    queue_destroy(q);

    open_dir(&d1, argv[1]);
    open_dir(&d2, argv[2]);

    print_dir_content(d1, argv[1]);
    print_dir_content(d2, argv[2]);

    closedir(d1);
    closedir(d2);

    printf("Done!\n");

    return 0;
}

void file_print(void *f) {
    struct file *file = (struct file *)f;
    printf("File name: %s File path: %s\n", file->name, file->path);
}

/* DIR */
void open_dir(DIR **dir, char *path) {
    if ((*dir = opendir(path)) == NULL) {
        printf("Unable to open dir %s\n", path);
        perror("");
        exit(EXIT_FAILURE);
    }
}

void print_dir_content(DIR *dir, char *dirname) {
    struct dirent *de;

    printf("Content of folder %s\n", dirname);

    while ((de = readdir(dir)) != NULL) {
        printf("\t%s -> %s\n", d_type_to_string(de->d_type), de->d_name);
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

/* QUEUE */
Queue queue_create() {
    Queue q;
    // TODO handle failure
    q = malloc(sizeof(*q));
    q->top = NULL;
    return q;
}

void queue_push(Queue q, Item i) {
    struct node *new;
    // TODO handle failure
    new = malloc(sizeof(*new));
    new->item = i;
    if (q->top == NULL) {
        q->top = new;
    } else {
        q->top->next = new;
    }
}

void queue_each(Queue q, void consumer(Item)) {
    struct node *top;

    if (q == NULL) {
        return;
    }

    top = q->top;
    while (top) {
        consumer(top->item);
        top = top->next;
    }
}

void queue_destroy(Queue q) {
    struct node *next, *top;

    if (q == NULL) {
        return;
    }

    top = q->top;
    while (top) {
        next = top->next;
        // TODO handle failure
        free(top);
        top = next;
    }

    // TODO handle failure
    free(q);
}
