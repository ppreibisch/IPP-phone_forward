#include <stdlib.h>
#include <string.h>

typedef struct Node Node;
/**
 * To jest struktura reprezentująca linked list.
 */
struct Node {
    char *data; /**< Tablica reprezentująca numer telefonu.*/
    Node *next; /**< Wskaźnik na kolejny element.*/
    Node *prev; /**< Wskaźnik na poprzedni element.*/
};


size_t listSize(Node *head) {
    size_t result = 0;
    while (head) {
        ++result;
        head = head->next;
    }
    return result;
}

void deleteNode(Node **head, Node *element) {
    if (!*head || !element)
        return;

    if (*head == element)
        *head = element->next;

    if (element->next)
        element->next->prev = element->prev;

    if (element->prev)
        element->prev->next = element->next;

    if (element->data)
        free(element->data);
    free(element);
}

Node *push(Node** head, char const *data) {
    if (!data) return NULL;
    Node* newNode = malloc(sizeof(Node));
    if (!newNode)
        return NULL;

    size_t size = strlen(data);
    newNode->data = (char *) malloc((size + 1) * sizeof(char));
    if (!newNode->data) {
        free(newNode);
        return NULL;
    }

    for (size_t i = 0; i <= size; ++i)
        newNode->data[i] = data[i];

    newNode->prev = NULL;
    newNode->next = (*head);

    if (*head)
        (*head)->prev = newNode;

    (*head) = newNode;
    return newNode;
}