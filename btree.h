#ifndef BTREE_H_INCLUDED
#define BTREE_H_INCLUDED

#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <string.h>
#include <stdint.h>

#define BUFFERSIZE 64
#define ID_SIZE BUFFERSIZE


#define COR_VERDE   "\033[1;32m"
#define COR_CIANO   "\033[1;36m"
#define COR_AMARELO "\033[1;33m"
#define COR_RESET   "\033[0m"
void setBaseDir(const char *path);



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
    uint64_t nextId;
    char rootId[BUFFERSIZE]; //ponteiro para a raiz
    char baseDir[256]; //adicionado pro conta dos testes para direcionar ao diretorio correto
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


bool saveMetadata(const Metadata *meta) ;
bool loadMetadata(Metadata *meta);

void printBtreeDFS(int T);

void printBtreeRec(btreeNode *Node,int T);

void printBtreeBFS(int T);

void removeCLRS(int key, int T);

int getPredecessor(btreeNode *Node, int T);
#endif // BTREE_H_INCLUDED



/*
typedef struct btreeNode{
    char id[BUFFERSIZE];
    int n;
    bool leaf;
    int *keys;
    char **children;
} btreeNode;


typedef struct SearchResult{
    btreeNode *node; // nó onde a chave foi encontrada
    int index;       // posição do keys em node->keys[index]
} searchResult;

typedef struct Metadata{
    uint64_t next_id;
    char root_id[BUFFERSIZE];
}Metadata;
*/
