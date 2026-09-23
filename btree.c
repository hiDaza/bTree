
#include "btree.h"


typedef struct btreeNode{
    int n;  //qtd de valores no nó
    int *keys;  //os valores do nó
    char **children; //nome dos arquivos
    bool leaf;
}btreeNode;


typedef struct SearchResult{
    btreeNode *node; // nó onde a chave foi encontrada
    int index;       // posição do keys em node->keys[index]
} searchResult;

btreeNode* createNode(int T, bool leaf){
    btreeNode* node = (btreeNode*) malloc(sizeof(btreeNode));
    node->n = 0;
    node->keys = (int*) malloc((2 * T - 1) * sizeof(int));
    node->children = (char**) malloc((2 * T) * sizeof(char));
    node->leaf = leaf;
    return node;
}



searchResult searchBTree(btreeNode *Node, int target){
    int start = 0;
    int end = Node->n -1;

    while(start <= end){

        int midle = (start + end) / 2;

        if(target == Node->keys[midle]){
            searchResult result;
            result.index = midle;
            result.node = Node;
            return result;
        }

        if(target > Node->keys[midle]){
            start = midle + 1;
            // return searchBTree(Node,start,end,target);
        }

        if(target < Node->keys[midle]){
            end = midle -1;
            //return searchBTree(Node,start,end,target);
        }
    }
    if(Node->leaf == true){
        return -1;
    }
    ///adicionar a leitura do nó filho aqui
    return searchBTree(, target); ///passar o filho do nó lido na função

}



