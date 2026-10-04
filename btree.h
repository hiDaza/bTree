#ifndef BTREE_H_INCLUDED
#define BTREE_H_INCLUDED

#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <string.h>
#include <stdint.h>

#define BUFFERSIZE 64
#define ID_SIZE BUFFERSIZE


typedef struct btreeNode {
    char id[BUFFERSIZE];
    int n;
    bool leaf;
    int *keys;
    char **children;
} btreeNode;

typedef struct SearchResult {
    btreeNode *node; // Nó onde a chave foi encontrada
    int index;       // Posição da chave em node->keys[index]
} searchResult;

typedef struct Metadata {
    uint64_t next_id;
    char root_id[BUFFERSIZE]; //ponteiro para a raiz
} Metadata;


btreeNode* createNode(int T, bool leaf, Metadata *meta);
void freeNode(btreeNode *node, int T);
char* allocateNode(btreeNode **newNode, int T, Metadata *meta);

void diskWrite(btreeNode *Node, int T);
btreeNode* diskRead(const char *Node_id, int T);

void numToBase62(uint64_t num, char *out);

searchResult searchBTree(btreeNode *Node, int target, int T);
void splitChild(btreeNode *parent, int index, btreeNode *fullChild, int T, Metadata *meta);
void insertCLRSNode(btreeNode *Node, int key, int T, Metadata *meta);
void insertNonFull(btreeNode *Node, int key, int T, Metadata *meta);

#endif // BTREE_H_INCLUDED
