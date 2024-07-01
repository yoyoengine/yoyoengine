/*
    This file is a part of yoyoengine. (https://github.com/zoogies/yoyoengine)
    Copyright (C) 2023  Ryan Zmuda

    Licensed under the MIT license. See LICENSE file in the project root for details.
*/

/**
 * @file linkedlist.h
 * @brief Unused linked list implementation
 * 
 * I'm not going to bother documenting this for now, seeing as its unused
 */

/*
    Usage Example:

    void printInt(void *data) {
        printf("%d ", *((int *)data));
    }

    int main() {
        LinkedList myList;
        initializeList(&myList);

        for (int i = 1; i <= 5; ++i) {
            int *num = (int *)malloc(sizeof(int));
            *num = i;
            insert(&myList, num);
        }

        printf("List elements: ");
        traverse(&myList, printInt);
        printf("\n");

        freeList(&myList);

        return 0;
    }

    This impl kinda sucks and is unused afaik
*/

#ifndef LINKEDLIST_H
#define LINKEDLIST_H

#include <stdlib.h>

/*
    Holds a void* to some data, and the next item in the list
*/
typedef struct Node {
    void *data;
    char *key;
    struct Node *next;
} Node;

/*
    Represents an actual list with a head node
*/
typedef struct LinkedList {
    Node *head;
} LinkedList;

/*
    Initializes a linked list
*/
void LL_initializeList(LinkedList *list) {
    list->head = NULL;
}

/*
    Throws a new item onto the head
    
    We dont really have a use case for implementing sorted insertion
    at the moment, anything using this generic will have negligible
    item counts
*/
void LL_insert(LinkedList *list, void *data, char *key) {
    Node *newNode = (Node *)malloc(sizeof(Node));
    newNode->data = data;
    newNode->next = list->head;
    newNode->key = key;
    list->head = newNode;
}

/*
    Traverses the list, runs a passed callback fn ptr for every item,
    passing each item as a parameter 
*/
void LL_traverse(LinkedList *list, void (*process)(void *)) {
    Node *current = list->head;
    while (current != NULL) {
        process(current);
        current = current->next;
    }
}

/*
    Attempts to delete an item from the list by key
*/
void LL_delete(LinkedList *list, char *key){
    Node *current = list->head;
    Node *prev = NULL;
    while (current != NULL) {
        if(current->key == key){
            if(prev == NULL){
                list->head = current->next;
            } else {
                prev->next = current->next;
            }
            free(current);
            return;
        }
        prev = current;
        current = current->next;
    }
}

/*
    Frees a linked list
*/
void LL_freeList(LinkedList *list) {
    Node *current = list->head;
    while (current != NULL) {
        Node *temp = current;
        current = current->next;
        free(temp);
    }
    list->head = NULL;
}

#endif

// this is cool but needs better ability to insert and delete