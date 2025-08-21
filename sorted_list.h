#ifndef SORTED_LIST_H

#define SORTED_LIST_H
typedef struct list *SortedList;
typedef void *Item;

SortedList list_create(int(Item, Item));
void list_push(SortedList, Item);
Item list_pop(SortedList);
void list_each(SortedList, void(Item));
void list_destroy(SortedList, void(Item));
#endif
