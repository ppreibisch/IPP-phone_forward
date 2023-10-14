/** @file
 * Interfejs klasy reprezentującej drzewo Trie
 *
 * @author Paweł Preibisch <pp438687@mimuw.edu.pl>
 * @copyright Uniwersytet Warszawski
 * @date 2022
 */

#ifndef __TRIE_H__
#define __TRIE_H__

#include <stdbool.h>
#include <stddef.h>
#include "linked_list.h"

#define N 12 /**< Ilość cyfr, służy do określenia ilości dzieci w drzewie. */

typedef struct TrieNode TrieNode;

/**
 * To jest struktura reprezentująca drzewo Trie.
 */
struct TrieNode {
    union {
        char *forward;
        Node *forwardsList;
    } data; /**< Przekierowanie numeru telefonu.*/
    TrieNode *child[N]; /**< Tablica dzieci. */
    TrieNode *father; /**< Wskaźnik na ojca. */
    TrieNode *reverseNode; /**< Wskaźnik na wierzchołek odpowiadający mu w drzewie reverseTrie. */
    Node *ptrToList; /**< Wskaźnik na element w liście w drzewie reverseTrie. */
    size_t lastIndex; /**< Indeks ostatniego usuniętego dziecka w funkcji remove. */
    bool isReverse; /**< Flaga mówiąca czy drzewo jest typu reverse. */
};

/** @brief Tworzy nową strukturę.
 * Otrzymuje parametr @p i tworzy nową strukturę bez żadnych wierzchołków tylko ze wskaźnikiem na korzeń.
 * W zależności od parametru @p drzewo jest typu Reverse lub Forward.
 * @return Wskaźnik na utworzoną strukturę lub NULL, gdy nie udało się
 *         alokować pamięci.
 */
TrieNode *trieNew(bool isReverse);

/** @brief Usuwa strukturę.
 * Usuwa strukturę wskazywaną przez @p root. Nic nie robi, jeśli wskaźnik ten ma wartość NULL.
 * @param[in] root – wskaźnik na usuwaną strukturę.
 */
void trieDelete(TrieNode **root);

/** @brief Dodaje przekierowanie numeru telefonu.
 * Tworzy poddrzewo zawierające reprezentujące @p num1 gdzie ostatni wierzchołek reprezentujący
 * ostatnią cyfrę numeru telefonu @p num1 trzyma informacje o przekierowaniu na @p num2.
 * @param[in] root – wskaźnik na strukturę reprezentująca drzewo Trie.
 * @param[in] num1 – wskaźnik na napis reprezentujący numer.
 * @param[in] num2 – wskaźnik na napis reprezentujący numer.
 * @return Wartość @p true, jeśli przekierowanie zostało dodane.
 *         Wartość @p false, jeśli wystąpił błąd, np. nie udało
 *         się alokować pamięci.
 */
TrieNode *trieAdd(TrieNode **root, char const *num1);

/** @brief Usuwanie przekierowanie numeru telefonu.
 * Usuwa całe poddrzewo którego początkowa ścieżka od korzenia jest reprezentacją
 * numeru @p num, usuwa je od ostatniego wierzchołka reprezentującego @p num aż do liści.
 * @param[in] root – wskaźnik na strukturę reprezentująca drzewo Trie.
 * @param[in] num – wskaźnik na napis reprezentujący numer.
 */
void trieRemove(TrieNode **root, char const *num);

/** @brief Zwraca przekierowanie numeru telefonu.
 * Zwraca wskaźnik na numer telefonu na który zostanie przekierowany @p num.
 * @param[in] root – wskaźnik na strukturę reprezentująca drzewo Trie.
 * @param[in] num – wskaźnik na napis reprezentujący numer.
 * @return Wskaźnik na napis reprezentujący numer telefonu który jest przekierowaniem @p num.
 */
char *trieFindForward(TrieNode *const *root, char const *num);

/** @brief Zwraca wskaźnik na tablice numerów.
 * Wskaźnik na tablice numerów które są wynikiem funkcji phfwdReverse dla danego numeru @p num
 * oraz drzewa przekierowań o korzeniu @p root.
 * @param[in] root – wskaźnik na strukturę reprezentująca drzewo reverseTrie.
 * @param[in] num – wskaźnik na napis reprezentujący numer.
 * @param[in] pnumSize - wskaźnik na liczbę odpowiadającą za rozmiar struktury PhoneNumbers,
 * która zostanie utworzona.
 * @return Wskaźnik na tablice numerów lub NULL, gdy nie udało się alokować pamięci.
 */
char **findReverseForwards(TrieNode *const *root, char const *num, size_t *pnumSize);

/** @brief Usuwa martwą ścieżkę.
 * Dla parametru @p node usuwa martwą ścieżkę tzn. taką która prowadzi od pewnego wierzchołka
 * z przekierowaniem do parametru @p node gdzie parametr @p node musi być liściem i po drodze
 * nie może być żadnych przekierowań.
 * @param[in] node - wskaźnik na wierzchołek.
 */
void deletePath(TrieNode *root);

/** @brief Usuwa wierzchołek.
 * Dla parametru @p node usuwa wszystkie jego informacje i zwalnia go z pamięci.
 * @param[in] node - wskaźnik na wierzchołek.
 */
void freeData(TrieNode *node);

#endif