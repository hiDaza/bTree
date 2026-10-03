#ifndef BTREE_H_INCLUDED
#define BTREE_H_INCLUDED

#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <string.h>
#include <stdint.h>
#define BUFFERSIZE 64
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


searchResult searchBTree(btreeNode *Node, int target);


btreeNode* splitChild(btreeNode *Node, int index);


btreeNode* insertCLRSNode(btreeNode *Node, int key);


void insertNonFull(btreeNode *Node, int key);


void numToBase62(uint64_t num, char *out);


char* allocateNode(btreeNode **newNode, int T, Metadata *meta);

#endif // BTREE_H_INCLUDED
