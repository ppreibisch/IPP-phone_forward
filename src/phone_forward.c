/** @file
 * Implementacja klasy przechowującej przekierowania numerów telefonicznych
 *
 * @author Paweł Preibisch <pp438687@mimuw.edu.pl>
 * @copyright Uniwersytet Warszawski
 * @date 2022
 */
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>
#include <ctype.h>
#include "trie.h"
#include "linked_list.h"

typedef struct PhoneForward PhoneForward;
/**
 * To jest struktura przechowująca przekierowania numerów telefonów.
 */
struct PhoneForward {
    TrieNode *forwardRoot; /**< Wskaźnik na drzewo Trie odpowiedzialne za działania na numerach telefonów. */
    TrieNode *reverseRoot; /**< Wskaźnik na drzewo Trie odpowiedzialne za operacje odwrócone na numerach telefonów. */
};

typedef struct PhoneNumbers PhoneNumbers;
/**
 * To jest struktura przechowująca ciąg numerów telefonów.
 */
struct PhoneNumbers {
    char **data; /**< Tablica numerów telefonów. */
    size_t size; /**< Ilość elementów znajdujących się w tablicy data. */
};

/** @brief Sprawdza poprawność numeru telefonu.
 * Funkcja sprawdzająca czy numer telefonu @p num jest poprawny.
 * @param num - wskaźnik na napis reprezentujący numer telefonu.
 * @return Wartość @p true gdy numer telefonu jest prawidłowy, @p false w przeciwnym przypadku.
 */
static bool isNumber(char const *num) {
    if (!num || strlen(num) == 0)
        return false;

    for (size_t i = 0; num[i] != '\0'; ++i) {
        if (!isdigit(num[i]) && num[i] != '#' && num[i] != '*')
            return false;
    }
    return true;
}

void phnumDelete(PhoneNumbers *pnum) {
    if (!pnum) return;
    for (size_t i = 0; i < pnum->size; ++i) {
        if (pnum->data[i])
            free(pnum->data[i]);
    }
    if (pnum->data)
        free(pnum->data);
    free(pnum);
}

PhoneNumbers *phfwdGet(PhoneForward const *pf, char const *num) {
    if (!pf) return NULL;
    PhoneNumbers *pnum = malloc(sizeof(struct PhoneNumbers));
    if (!pnum) return NULL;

    pnum->size = 0;
    pnum->data = NULL;
    if (!isNumber(num))
        return pnum;

    pnum->data = malloc(1 * sizeof(char*));
    if (!pnum->data) {
        free(pnum);
        return NULL;
    }

    char *res = trieFindForward(&(pf->forwardRoot), num);
    if (!res){
        phnumDelete(pnum);
        return NULL;
    }

    size_t size = strlen(res);
    pnum->data[pnum->size] = malloc((size + 1) * sizeof(char));
    if (!pnum->data[pnum->size]) {
        free(res);
        phnumDelete(pnum);
        return NULL;
    }

    for (size_t i = 0; i <= size; ++i)
        pnum->data[pnum->size][i] = res[i];
    ++pnum->size;

    if (res != num)
        free(res);
    return pnum;
}

char const *phnumGet(PhoneNumbers const *pnum, size_t idx) {
    if (!pnum || idx >= pnum->size)
        return NULL;
    return pnum->data[idx];
}

void phfwdDelete(PhoneForward *pf) {
    if (pf) {
        trieDelete(&(pf->forwardRoot));
        free(pf->forwardRoot);
        free(pf->reverseRoot);
        pf->forwardRoot = NULL;
        pf->reverseRoot = NULL;
        free(pf);
    }
}

PhoneForward *phfwdNew(void) {
    PhoneForward *phoneForward = malloc(sizeof(struct PhoneForward));

    if (phoneForward) {
        phoneForward->forwardRoot = trieNew(false);
        if (!phoneForward->forwardRoot) {
            free(phoneForward);
            return NULL;
        }
        phoneForward->reverseRoot = trieNew(true);
        if (!phoneForward->reverseRoot) {
            free(phoneForward->forwardRoot);
            free(phoneForward);
            return NULL;
        }
    }

    return phoneForward;
}

bool phfwdAdd(PhoneForward *pf, char const *num1, char const *num2) {
    if (pf && pf->reverseRoot && pf->forwardRoot && isNumber(num1) && isNumber(num2) && strcmp(num1, num2) != 0) {
        TrieNode *forwardPtr = trieAdd(&(pf->forwardRoot), num1);
        if (!forwardPtr)
            return false;

        TrieNode *reversePtr = trieAdd(&(pf->reverseRoot), num2);
        if (!reversePtr) {
            freeData(forwardPtr);
            deletePath(forwardPtr);
            return false;
        }

        forwardPtr->reverseNode = reversePtr;
        forwardPtr->ptrToList = push(&(reversePtr->data.forwardsList), num1);
        if (!forwardPtr->ptrToList) {
            freeData(forwardPtr);
            deletePath(forwardPtr);
            return false;
        }

        size_t size = strlen(num2);
        forwardPtr->data.forward = malloc((size + 1) * sizeof(char));
        if (!forwardPtr->data.forward) {
            freeData(forwardPtr);
            deletePath(forwardPtr);
            return false;
        }

        for (size_t i = 0; i <= size; ++i)
            forwardPtr->data.forward[i] = num2[i];
        return true;
    }

    return false;
}

void phfwdRemove(PhoneForward *pf, char const *num) {
    if (pf && pf->forwardRoot && isNumber(num))
        trieRemove(&(pf->forwardRoot), num);
}

PhoneNumbers *phfwdReverse(PhoneForward const *pf, char const *num) {
    if (!pf) return NULL;
    PhoneNumbers *pnum = malloc(sizeof(struct PhoneNumbers));
    if (!pnum) return NULL;

    pnum->size = 0;
    pnum->data = NULL;
    if (!isNumber(num))
        return pnum;

    size_t size = 0;
    char **res = findReverseForwards(&(pf->reverseRoot), num, &size);
    if (!res) {
        free(pnum);
        return NULL;
    }

    pnum->size = 0;
    pnum->data = malloc(size * sizeof(char *));
    if (!pnum->data) {
        for (size_t j = 0; j < size; ++j)
            free(res[j]);
        free(res);
        free(pnum);
        return NULL;
    }

    for (size_t i = 0; i < size; ++i) {
        size_t tempSize = strlen(res[i]);
        pnum->data[i] = malloc((tempSize + 1) * sizeof(char));
        if (!pnum->data[i]) {
            phnumDelete(pnum);

            for (size_t j = 0; j < size; ++j)
                free(res[j]);
            free(res);

            return NULL;
        }

        for (size_t j = 0; j <= tempSize; ++j)
            pnum->data[i][j] = res[i][j];
        ++pnum->size;
    }

    for (size_t j = 0; j < size; ++j)
        free(res[j]);
    free(res);

    return pnum;
}

PhoneNumbers *phfwdGetReverse(PhoneForward const *pf, char const *num) {
    if (!pf) return NULL;
    PhoneNumbers *pnum = malloc(sizeof(struct PhoneNumbers));
    if (!pnum) return NULL;

    pnum->size = 0;
    pnum->data = NULL;
    if (!isNumber(num))
        return pnum;

    size_t size = 0;
    char **tempResult = findReverseForwards(&(pf->reverseRoot), num, &size);
    if (!tempResult) {
        free(pnum);
        return NULL;
    }

    char *temp;
    size_t newSize = 0;
    for (size_t i = 0; i < size; ++i) {
        temp = trieFindForward(&(pf->forwardRoot), tempResult[i]);
        if (!temp) {
            for (size_t j = 0; j < size; ++j)
                free(tempResult[j]);
            free(tempResult);
            free(pnum);

            return NULL;
        }
        if (strcmp(temp, num) == 0)
            ++newSize;
        if (temp != tempResult[i])
            free(temp);
    }

    pnum->size = 0;
    pnum->data = malloc(newSize * sizeof(char *));
    if (!pnum->data) {
        for (size_t j = 0; j < size; ++j)
            free(tempResult[j]);
        free(tempResult);
        free(pnum);

        return NULL;
    }

    for (size_t i = 0; i < size; ++i) {
        temp = trieFindForward(&(pf->forwardRoot), tempResult[i]);
        if (!temp) {
            phnumDelete(pnum);
            for (size_t j = 0; j < size; ++j)
                free(tempResult[j]);
            free(tempResult);

            return NULL;
        }
        if (strcmp(temp, num) == 0) {
            size_t tempSize = strlen(tempResult[i]);
            pnum->data[pnum->size] = malloc((tempSize + 1) * sizeof(char));
            if (!pnum->data[pnum->size]) {
                phnumDelete(pnum);
                if (temp != tempResult[i])
                    free(temp);

                for (size_t j = 0; j < size; ++j)
                    free(tempResult[j]);
                free(tempResult);

                return NULL;
            }
            for (size_t j = 0; j <= tempSize; ++j)
                pnum->data[pnum->size][j] = tempResult[i][j];
            ++pnum->size;
        }
        if (temp != tempResult[i])
            free(temp);
    }
    for (size_t j = 0; j < size; ++j)
        free(tempResult[j]);
    free(tempResult);

    return pnum;
}