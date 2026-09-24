#ifndef BTREE_H_INCLUDED
#define BTREE_H_INCLUDED

#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>

typedef struct btreeNode{
    int n;
    int *keys;
    char **children;
    bool leaf;
}btreeNode;


typedef struct SearchResult{
    btreeNode *node; // nó onde a chave foi encontrada
    int index;       // posição do keys em node->keys[index]
} searchResult;


btreeNode* createNode(int T, bool leaf);


#endif // BTREE_H_INCLUDED
