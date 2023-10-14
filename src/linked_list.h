/** @file
 * Implementacja klasy reprezentującej linked list
 *
 * @author Paweł Preibisch <pp438687@mimuw.edu.pl>
 * @copyright Uniwersytet Warszawski
 * @date 2022
 */
#ifndef __LINKED_LIST_H__
#define __LINKED_LIST_H__

typedef struct Node Node;
/**
 * To jest struktura reprezentująca linked list.
 */
struct Node {
    char *data; /**< Tablica reprezentująca numer telefonu.*/
    Node *next; /**< Wskaźnik na kolejny element.*/
    Node *prev; /**< Wskaźnik na poprzedni element.*/
};

/** @brief Zwraca rozmiar listy.
 * Zwraca rozmiar listy której pierwszym elementem jest @p head.
 * @param[in] head – wskaźnik na pierwszy element.
 * @return Zwraca rozmiar listy której pierwszym elementem jest @p head.
 */
size_t listSize(Node *head);

/** @brief Dodaje element do listy.
 * Dodaje element element do listy który zawiera informacje @p data.
 * @param[in] head – wskaźnik na listę czyli na wskaźnik pierwszego elementu.
 * @param[in] data - informacja którą będzie zawierał nowy element.
 * @return zwraca nowo dodany wierzchołek lub NULL w przypadku
 * gdy nie udało się alokować pamięci.
 */
Node *push(Node **head, char const *data);

/** @brief Usuwa element z listy.
 * Usuwa element @p element z listy.
 * @param[in] head – wskaźnik na liste czyli na wskaźnik pierwszego elementu.
 * @param[in] element - wskaźnik na usuwany element.
 */
void deleteNode(Node **head, Node *element);

#endif
