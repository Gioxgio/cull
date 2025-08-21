#include "sorted_list.h"
#include <stdlib.h>

struct list {
    struct node *top;
    int (*cmp)(Item, Item);
};
struct node {
    Item item;
    struct node *next;
};

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
