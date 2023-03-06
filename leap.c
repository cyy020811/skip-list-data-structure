/*
leap.c

Implementations for leap list construction and manipulation.

Skeleton written by Grady Fitzaptrick for COMP20007 Assignment 1 2022
*/
#include <stdio.h>
#include <stdlib.h>
#include <assert.h>
#include "leap.h"
#include "utils.h"

struct node {
    int data;
    struct node* next;
    struct node* drop;
};

struct leapList {
    /* IMPLEMENT: Fill in structure. */
    int level;
    int maxHeight;
    double p;
    struct node* head;
    struct leapList* above;
    struct leapList* below;
    enum problemPart part;
};

struct node* newNode(int data) {
    /* Assigned memory space to the node */
    struct node* newNode = (struct node*)malloc(sizeof(struct node));
    assert(newNode != NULL);
    newNode->data = data;
    newNode->next = newNode->drop = NULL;
    return newNode;
}

struct leapList* newList(int maxHeight, double p, enum problemPart part) {
    /* IMPLEMENT: Set up list */
    struct leapList* newList = NULL;
    struct leapList* below = NULL;
    struct node* drop = NULL;
    int level = 1;
    /* Create a certain number of list according to the max height. */
    for (int i = 0; i < maxHeight; i++) {
        newList = (struct leapList*)malloc(sizeof(struct leapList));
        /* Give the new list a head node */
        struct node* headNode = (struct node*)malloc(sizeof(struct node));
        headNode->next = NULL;
        headNode->drop = NULL;
        newList->head = headNode;
        newList->level = level;
        newList->p = p;
        newList->maxHeight = maxHeight;
        newList->part = part;
        newList->below = below;
        newList->head->drop = drop;
        /* Link the current list and the bottom one */
        below = newList;
        drop = newList->head;
        level++;
    }
    return newList;
}

void printLevel(struct leapList* list, int level) {
    if (!list) {
        printf("\n");
        return;
    }
    /* IMPLEMENT (Part B): loop over list at given level, printing out each value. */
    struct leapList* currList = list;
    struct node* curr = NULL;
    /* Move to the given level */
    while (currList->level != level) currList = currList->below;
    curr = currList->head->next;
    /* Print out all the nodes at the level */
    while (curr != NULL) {
        printf("%d", curr->data);
        if (curr->next == NULL) break;
        printf(" ");
        curr = curr->next;
    }
    /* Note: while additional next elements, print a space after the key. If no additional next elements,
    print a new line and then return. */
    printf("\n");
    return;
}

void insertKey(int key, struct leapList* list) {
    /* IMPLEMENT: Insert the key into the given leap list. */
    struct node* drop = NULL;
    int insertedLevel = 0;
    int insert = 1;
    /* Execute when we have to insert */
    while (insert) {
        struct node* keyNode = newNode(key);
        struct leapList* currList = list;
        struct node* curr = list->head;
        int inserted = 0;
        /* Insert the key at the correct level and position */
        while (!inserted) {
            if (curr->next == NULL) {
                if (currList->level != insertedLevel + 1) {
                    curr = curr->drop;
                    currList = currList->below;
                }
                else {
                    keyNode->next = curr->next;
                    curr->next = keyNode;
                    keyNode->drop = drop;
                    drop = keyNode;
                    inserted = 1;
                    insertedLevel++;
                }
            }
            else if (curr->next->data > key) {
                if (currList->level != insertedLevel + 1) {
                    curr = curr->drop;
                    currList = currList->below;
                }
                else {
                    keyNode->next = curr->next;
                    curr->next = keyNode;
                    keyNode->drop = drop;
                    drop = keyNode;
                    inserted = 1;
                    insertedLevel++;
                }
            }
            else if (curr->next->data < key) curr = curr->next;
        }
        /* Check if the max heaigh has already been reached */
        if (insertedLevel == list->maxHeight) break;
        insert = (double)rand() / RAND_MAX < list->p;
    }
}

/* Queries the leap list for the given key and places the result in the solution structure. */
int findKey(int key, struct leapList* list, enum problemPart part, struct solution* solution) {
    int found = NOTFOUND;
    int element = key;
    int baseAccesses = 0;
    int requiredAccesses = 0;
    assert(solution);
    /* IMPLEMENT: Find the given key in the leap list. */
    struct node* curr = NULL;
    struct leapList* currList = NULL;
    currList = list;
    curr = currList->head;
    while (found == NOTFOUND) {
        if (currList->head->next == NULL) {
            currList = currList->below;
            curr = currList->head;
        }
        /* If the next node is NULL then break */
        else if (curr->next == NULL) {
            if (currList->level == 1) {
                break;
            }
            /* Dropdown step */
            else {
                curr = curr->drop;
                currList = currList->below;
            }
        }
        /* If the next node equals to the key */
        else if (curr->next->data == key) {
            requiredAccesses++;
            found = 1;
        }
        /* If the next node is greater than the key */
        else if (curr->next->data > key) {
            /* Abort if the current level is the lowest one*/
            if (currList->level == 1) {
                requiredAccesses++;
                break;
            }
            /* Dropdown step */
            else {
                if (curr->drop->next->data != curr->next->data) requiredAccesses++;
                curr = curr->drop;
                currList = currList->below;
            }
        }
        /* Scan forward step */
        else {
            curr = curr->next;
            requiredAccesses++;
        }
    }

    /* Look for the base accesses */
    while (currList->below != NULL) currList = currList->below;
    struct node* baseCurr = currList->head->next;

    while (baseCurr != NULL) {
        if (baseCurr->data >= key) {
            baseAccesses++;
            break;
        }
        baseAccesses++;
        baseCurr = baseCurr->next;
    }


    /* Insert result into solution. */
    (solution->queries)++;
    solution->queryResults = (int*)realloc(solution->queryResults, sizeof(int) * solution->queries);
    assert(solution->queryResults);
    (solution->queryResults)[solution->queries - 1] = found;
    solution->queryElements = (int*)realloc(solution->queryElements, sizeof(int) * solution->queries);
    assert(solution->queryElements);
    solution->queryElements[solution->queries - 1] = element;
    solution->baseAccesses = (int*)realloc(solution->baseAccesses, sizeof(int) * solution->queries);
    assert(solution->baseAccesses);
    solution->baseAccesses[solution->queries - 1] = baseAccesses;
    solution->requiredAccesses = (int*)realloc(solution->requiredAccesses, sizeof(int) * solution->queries);
    assert(solution->requiredAccesses);
    solution->requiredAccesses[solution->queries - 1] = requiredAccesses;
    return found;
}

void deleteKey(int key, struct leapList* list, enum problemPart part) {
    /* IMPLEMENT: Remove the given key from the leap list. */
    int found = 1;
    struct leapList* currList = NULL;
    struct node* curr = NULL;
    struct node* next = NULL;
    struct node* temp = NULL;
    currList = list;
    curr = currList->head;
    /* Delete the key level by level */
    while (found != NOTFOUND) {
        if (currList->head->next == NULL) {
            if (currList->level == 1) found = NOTFOUND;
            else {
                currList = currList->below;
                curr = currList->head;
            }
        }
        else if (curr->next == NULL) {
            if (currList->level == 1) found = NOTFOUND;
            else {
                curr = curr->drop;
                currList = currList->below;
            }
        }
        /* Delete the key and free the node */
        else if (curr->next->data == key) {
            temp = curr->next;
            next = temp->next;
            curr->next = next;
            free(temp);
            temp = NULL;
            next = NULL;
            currList = list;
            curr = currList->head;
        }
        else if (curr->next->data > key) {
            if (currList->level == 1) found = NOTFOUND;
            else {
                curr = curr->drop;
                currList = currList->below;
            }
        }
        else {
            curr = curr->next;
        }
    }
}

void freeList(struct leapList* list) {
    /* IMPLEMENT: Free all memory used by the list. */
    struct node* curr = NULL;
    struct node* temp = NULL;
    /* Use recursion to free the bottom list first */
    if (list->below != NULL) freeList(list->below);
    curr = list->head;
    /* Free the nodes */
    while (curr != NULL) {
        temp = curr;
        curr = curr->next;
        free(temp);
        temp = NULL;
    }
    /* Free the list */
    free(list);
}

void freeSolution(struct solution* solution) {
    if (!solution) {
        return;
    }
    freeList(solution->list);
    if (solution->queries > 0) {
        free(solution->queryResults);
        free(solution->queryElements);
        free(solution->baseAccesses);
        free(solution->requiredAccesses);
    }
    free(solution);
}