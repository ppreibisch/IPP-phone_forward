/** @file
 * Implementacja klasy reprezentującej drzewo Trie
 *
 * @author Paweł Preibisch <pp438687@mimuw.edu.pl>
 * @copyright Uniwersytet Warszawski
 * @date 2022
 */
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>
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

/** @brief Zwraca czy wierzchołek przechowuje jakiekolwiek dane.
 * Funkcja zwraca czy parametr @p node ma jakieś dane.
 * @param[in] node - wskaźnik na wierzchołek.
 * @return Wartość @p true w przypadku gdy parametr @p node zawiera
 * jakieś informacje a wartość @p false w przeciwnym razie.
 */
static bool checkData(TrieNode *node) {
    return node->data.forwardsList || node->data.forward;
}

/** @brief Zwraca indeks w tablicy dla chara.
 * Funkcja zwraca indeks w tablicy child dla parametru @p c.
 * @param[in] c - litera.
 * @return Indeks odpowiadający parametrowi @p c w tablicy child.
 */
static int findIndex(char c) {
    if (c == '*')
        return 10;
    else if (c == '#')
        return 11;
    else
        return (int) c - '0';
}

/** @brief Dla danego wierzchołka zwraca którym dzieckiem jest dla swojego ojca.
 * Funkcja zwraca indeks który w tablicy child dla swojego ojca jest parametr @p node.
 * @param[in] node - wskaźnik na wierzchołek.
 * @return Indeks odpowiadający którym dzieckiem jest parametr @p node dla swojego ojca.
 */
static int findChildIndex(TrieNode *node) {
    int idx = -1;
    for (int i = 0; i < N; ++i) {
        if (node->father->child[i] == node)
            idx = i;
    }
    return idx;
}

/** @brief Sprawdza czy wierzchołek posiada jakieś dzieci.
 * Sprawdza czy wierzchołek @p node posiada jakieś dzieci.
 * @param[in] node - wskaźnik na wierzchołek.
 * @return Wartość @p true w przypadku gdy @p node zawiera
 * jakieś dzieci a wartość @p false w przeciwnym razie.
 */
static bool noChild(TrieNode *node) {
    for (int i = 0; i < N; ++i) {
        if (node->child[i])
            return false;
    }
    return true;
}

void deletePath(TrieNode *root) {
    TrieNode *ptr = root;
    if (!ptr) return;

    while (ptr->father && !checkData(ptr) && noChild(ptr)) {
        int position = findChildIndex(ptr);
        TrieNode *temp = ptr->father;
        ptr->father = NULL;
        free(ptr);
        temp->child[position] = NULL;
        ptr = temp;
    }
}

void freeData(TrieNode *node) {
    if (!node)
        return;
    if (!node->isReverse) {
        if (node->reverseNode) {
            deleteNode(&(node->reverseNode->data.forwardsList), node->ptrToList);
            deletePath(node->reverseNode);
            node->reverseNode = NULL;
            node->ptrToList = NULL;
        }
        if (node->data.forward) {
            free(node->data.forward);
            node->data.forward = NULL;
        }
    }
}

TrieNode *trieNew(bool isReverse) {
    TrieNode *trieNode = malloc(sizeof(struct TrieNode));

    if (trieNode) {
        for (size_t i = 0; i < N; ++i)
            trieNode->child[i] = NULL;
        trieNode->father = NULL;
        trieNode->lastIndex = 0;
        trieNode->isReverse = isReverse;
        if (isReverse)
            trieNode->data.forwardsList = NULL;
        else
            trieNode->data.forward = NULL;
        trieNode->reverseNode = NULL;
    }

    return trieNode;
}

void trieDelete(TrieNode **root) {
    TrieNode *ptr = *root;
    while (ptr) {
        for (size_t i = ptr->lastIndex; i < N; ++i) {
            if (ptr->child[i]) {
                ptr->lastIndex = i;
                break;
            }
        }
        if (!ptr->child[ptr->lastIndex]) {
            if (!ptr->father)
                break;
            ptr = ptr->father;
            freeData(ptr->child[ptr->lastIndex]);
            free(ptr->child[ptr->lastIndex]);
            ptr->child[ptr->lastIndex] = NULL;
        } else
            ptr = ptr->child[ptr->lastIndex];
    }
    freeData(*root);
    free(*root);
    *root = NULL;
}


TrieNode *trieAdd(TrieNode **root, char const *num) {
    TrieNode *ptr = *root;

    int position;
    for (size_t i = 0; num[i] != '\0'; ++i) {
        position = findIndex(num[i]);
        if (!ptr->child[position]) {
            ptr->child[position] = trieNew(ptr->isReverse);
            if (!ptr->child[position]) return false;
            ptr->child[position]->father = ptr;
        }
        ptr = ptr->child[position];
    }
    freeData(ptr);

    return ptr;
}

void trieRemove(TrieNode **root, char const *num) {
    TrieNode *ptr = *root;

    int position = -1;
    for (size_t i = 0; num[i] != '\0'; ++i) {
        position = findIndex(num[i]);
        if (!ptr->child[position])
            return;
        ptr = ptr->child[position];
    }
    if (position == -1)
        return;

    TrieNode *temp = ptr->father;
    ptr->father = NULL;
    trieDelete(&ptr);
    temp->child[position] = NULL;
    deletePath(temp);
}

char *trieFindForward(TrieNode *const *root, char const *num) {
    TrieNode *ptr = *root;
    if (!ptr) return NULL;
    TrieNode *res = NULL;

    int position;
    size_t len = 0;
    for (size_t i = 0; num[i] != '\0'; ++i) {
        position = findIndex(num[i]);
        if (!ptr->child[position])
            break;
        ptr = ptr->child[position];
        if (checkData(ptr)) {
            res = ptr;
            len = i + 1;
        }
    }

    if (res) {
        size_t size1 = strlen(num);
        size_t size2 = strlen(res->data.forward);
        char *info = malloc((size1 - len + size2 + 1) * sizeof(char));
        if (!info)
            return NULL;
        for (size_t i = 0; i < size2; ++i)
            info[i] = res->data.forward[i];
        for (size_t i = len; i <= size1; ++i)
            info[i - len + size2] = num[i];
        return info;
    }
    return (char *) num;
}


static int comparator(const void *a, const void *b) {
    char *num1 = *(char **) a;
    char *num2 = *(char **) b;
    size_t pos = 0;

    while (true) {
        if (num1[pos] == '\0' && num2[pos] == '\0')
            return 0;
        else if (num1[pos] == '\0')
            return -1;
        else if (num2[pos] == '\0')
            return 1;
        else if (num1[pos] == num2[pos])
            pos++;
        else
            return findIndex(num1[pos]) - findIndex(num2[pos]);
    }
}

/** @brief Liczy rozmiar tablicy reverse.
 * Dla parametru @p root i numeru @p num liczy ile jest numerów których
 * przekierowanie według definicji reverse daje w wyniku @p num.
 * @param[in] root - wskaźnik na korzeń.
 * @param[in] num - wskaźnik na numer.
 * @return Wartość ile numerów jest przekierowywanych na @p num,
 * według definicji reverse.
 */
static size_t countSize(TrieNode *const *root, char const *num) {
    TrieNode *ptr = *root;

    int position;
    size_t size = 1;
    for (size_t i = 0; num[i] != '\0'; ++i) {
        position = findIndex(num[i]);
        if (!ptr->child[position])
            break;
        ptr = ptr->child[position];
        size += listSize(ptr->data.forwardsList);
    }
    return size;
}

char **findReverseForwards(TrieNode *const *node, char const *num, size_t *pnumSize) {
    TrieNode *ptr = *node;
    if (!ptr) return NULL;

    size_t size = countSize(node, num), numLength = strlen(num);
    char **arr = malloc(size * sizeof(char *));
    if (!arr)
        return NULL;

    arr[size - 1] = malloc((numLength + 1) * sizeof(char));
    if (!arr[size - 1]) {
        free(arr);
        return NULL;
    }
    for (size_t i = 0; i <= numLength; ++i)
        arr[size - 1][i] = num[i];

    int position;
    size_t idx = 0;
    for (size_t i = 0; num[i] != '\0'; ++i) {
        position = findIndex(num[i]);
        if (!ptr->child[position])
            break;

        ptr = ptr->child[position];
        if (ptr->data.forwardsList) {
            Node *head = ptr->data.forwardsList;
            while (head) {
                size_t tempLen = strlen(head->data);
                arr[idx] = malloc((tempLen + numLength - i) * sizeof(char));
                if (!arr[idx]) {
                    for (size_t j = 0; j < idx; ++j)
                        free(arr[j]);
                    if (size > idx)
                        free(arr[size - 1]);
                    free(arr);

                    return NULL;
                }
                for (size_t j = 0; j < tempLen; ++j)
                    arr[idx][j] = head->data[j];
                for (size_t j = tempLen; j <= tempLen + numLength - i - 1; ++j)
                    arr[idx][j] = num[i + 1 + j - tempLen];
                ++idx;
                head = head->next;
            }
        }
    }

    qsort(arr, size, sizeof(char *), comparator);

    size_t repeats = 0;
    for (size_t i = 1; i < size; ++i) {
        if (strcmp(arr[i], arr[i - 1]) == 0)
            ++repeats;
    }

    *pnumSize = size - repeats;
    char **result = (char **) malloc(*pnumSize * sizeof(char *));

    if (!result) {
        for (size_t j = 0; j < size; ++j)
            free(arr[j]);
        free(arr);
        return NULL;
    }

    idx = 0;
    for (size_t i = 0; i < size; ++i) {
        if (i == 0 || strcmp(arr[i], arr[i - 1]) != 0) {
            numLength = strlen(arr[i]);
            result[idx] = (char *) malloc((numLength + 1) * sizeof(char));
            if (!result[idx]) {
                for (size_t j = 0; j < idx; ++j)
                    free(result[j]);
                free(result);

                for (size_t j = 0; j < size; ++j)
                    free(arr[j]);
                free(arr);

                return NULL;
            }
            for (size_t j = 0; j <= numLength; ++j)
                result[idx][j] = arr[i][j];
            ++idx;
        }
    }

    for (size_t j = 0; j < size; ++j)
        free(arr[j]);
    free(arr);
    return result;
}
