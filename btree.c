
#include "btree.h"



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
        return ;
    }
    ///adicionar a leitura do nó filho aqui
    return searchBTree(, target); ///passar o filho do nó lido na função
}



btreeNode* splitChild(btreeNode *Node, int index){
    btreeNode *rightNode = createNode(T,1);
    rightNode->leaf = fullChild->leaf;
    bTreeNode *parent = Node;
    bTreeNode *fullChild = Node->children[index];

    int start = 0;
    int end = fullChild->n - 1;
    int midle = (start + end) / 2;
    int startRigth = 0;

    for(int i = midle + 1; i <=  end; i++){
        rightNode->keys[startRight] = fullChild->keys[i];
        startRight++;

    }

    if (!fullChild->leaf) {
        for (int j = midle + 1; j <= end + 1; j++) {
            rightNode->children[j - (midle + 1)] = fullChild->children[j];
        }
    }

    rightNode->n = startRigth;
    fullChild->n = midle;

    for(int i = parent->n -1; i >= index; i--){
            parent->keys[i+1] = parent->keys[i];

    }
    parent->keys[index] = fullchild->keys[midle];
    parent->n = parent->n+1;

    for(int i = parent->n-1; i > index; i--){
        parent->children[i+1] = parent->children[i];
    }
    parent->children[index+1] = rightNode;
}


