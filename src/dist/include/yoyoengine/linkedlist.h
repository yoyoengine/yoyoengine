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
*/

#ifndef LINKEDLIST_H
#define LINKEDLIST_H

/*
    Holds a void* to some data, and the next item in the list
*/
typedef struct Node {
    void *data;
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
void LL_insert(LinkedList *list, void *data) {
    Node *newNode = (Node *)malloc(sizeof(Node));
    newNode->data = data;
    newNode->next = list->head;
    list->head = newNode;
}

/*
    Traverses the list, runs a passed callback fn ptr for every item,
    passing each item as a parameter 
*/
void LL_traverse(LinkedList *list, void (*process)(void *)) {
    Node *current = list->head;
    while (current != NULL) {
        process(current->data);
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