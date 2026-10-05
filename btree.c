
#include "btree.h"



btreeNode* createNode(int T, bool leaf, Metadata *meta){
    btreeNode* node = (btreeNode*) malloc(sizeof(btreeNode));

    numToBase62(meta->next_id++, node->id);

    node->n = 0;
    node->leaf = leaf;
    node->keys = (int*) malloc((2 * T - 1) * sizeof(int));
    node->children = (char**) malloc((2 * T) * sizeof(char*));

    for(int i = 0; i < 2 * T; i++){
        node->children[i] = (char*) malloc(sizeof(char*) * BUFFERSIZE);
        node->children[i][0] = '\0';
    }

    return node;
}

void freeNode(btreeNode *node, int T) {
    if (node == NULL) return;
    if (node->keys) free(node->keys);
    if (node->children) {
        for (int i = 0; i < 2 * T; i++) {
            if (node->children[i]) free(node->children[i]);
        }
        free(node->children);
    }
    free(node);
}



searchResult searchBTree(btreeNode *Node, int target,int T){
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
            // return bussearchBTree(Node,start,end,target);
        }

        if(target < Node->keys[midle]){
            end = midle -1;
            //return searchBTree(Node,start,end,target);
        }
    }
    if(Node->leaf){
        searchResult notFound;
        notFound.index = -1;
        notFound.node = NULL;
        return notFound; //usar outra operação para finalizar talvez para nao precisar alocar uma nova struct
    }
    btreeNode *childNode = diskRead(Node->children[start],T);

    searchResult result = searchBTree(childNode, target, T);

    return result;
}



void splitChild(btreeNode *parent, int index, btreeNode *fullChild, int T, Metadata *meta){
    btreeNode *rightNode = createNode(T,fullChild->leaf,meta);

    int start = 0;
    int end = fullChild->n - 1;
    int midle = (start + end) / 2;
    int startRigth = 0;

    for(int i = midle + 1; i <=  end; i++){
        rightNode->keys[startRigth] = fullChild->keys[i];
        startRigth++;
    }

    if (!fullChild->leaf) {
        for (int j = midle + 1; j <= fullChild->n; j++) {
            strncpy(rightNode->children[j - (midle + 1)], fullChild->children[j], BUFFERSIZE - 1);
        }
    }

    rightNode->n = startRigth;
    fullChild->n = midle;

    for (int i = parent->n; i > index; i--) {
        strncpy(parent->children[i + 1], parent->children[i], BUFFERSIZE - 1);
    }

    strncpy(parent->children[index + 1], rightNode->id, BUFFERSIZE - 1);

    for (int i = parent->n - 1; i >= index; i--) {
        parent->keys[i + 1] = parent->keys[i];
    }
    parent->keys[index] = fullChild->keys[midle];
    parent->n++;

    diskWrite(fullChild, T);
    diskWrite(rightNode, T);
    diskWrite(parent, T);

    freeNode(rightNode, T);
}


void insertCLRSNode(btreeNode *Node, int key,int T,Metadata *meta){
    int start = 0;
    int end = Node->n -1;
    int midle = (start + end) / 2;
    if(Node->n >= 2 * T - 1){ //inicia verificando se a raiz esta cheia
        btreeNode *newRoot; //criação de um novo nó para receber a raiz cheia para possibilitar o split child na raiz
        newRoot = createNode(T,false,meta);
        strncpy(newRoot->children[0],Node->id,BUFFERSIZE-1);
        splitChild(newRoot,0,Node,T,meta);
        strncpy(meta->root_id,newRoot->id, BUFFERSIZE-1); //com a criação do newRoot agora existe uma nova raiz que deve ser salva no metadata
        insertNonFull(newRoot,key,T,meta);
        freeNode(newRoot,T);
        freeNode(Node,T);
    }else{
        insertNonFull(Node,key,T,meta);
        freeNode(Node,T);
    }

}


void insertNonFull(btreeNode *Node, int key, int T,Metadata *meta){
    int s = 0;
    if(Node->leaf){
        int i = Node->n-1;
            while(i >= 0 && Node->keys[i] > key){
                Node->keys[i+1] = Node->keys[i];
                i--;
            }
        Node->keys[i+1] = key;
        Node->n++;
        diskWrite(Node,T);
    }else{
        int i = Node->n-1;
            while(i >= 0 && Node->keys[i] > key){
                i--;
            }
            i++;
            btreeNode *child = diskRead(Node->children[i],T);
            if(child->n >= 2 * T-1){
                splitChild(Node,i,child,T,meta);
                if(key > Node->keys[i]){
                    i++;
                    freeNode(child,T);
                    child = diskRead(Node->children[i],T);
                }
            }
            insertNonFull(child,key,T,meta);
            freeNode(child,T);
    }
}



void numToBase62(uint64_t num, char *out) {
    const char charset[] = "0123456789abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ";
    if (num == 0) {
        out[0] = '0';
        out[1] = '\0';
        return;
    }

    char temp[64];
    int i = 0;
    while (num > 0) {
        temp[i++] = charset[num % 62];
        num /= 62;
    }

    int j = 0;
    while (i > 0) {
        out[j++] = temp[--i];
    }
    out[j] = '\0';
}

/*
char* allocateNode(btreeNode **newNode, int T, Metadata *meta) {
    uint64_t raw_id;

    if (meta->free_count > 0) {
        raw_id = meta->free_ids[--meta->free_count];
    } else {
        raw_id = meta->next_id++;
    }

    char *id_str = (char*) malloc(sizeof(char) * 64);
    numToBase62(raw_id, id_str);

    newNode* = (btreeNode*) malloc(sizeof(btreeNode));
    newNode*->n = 0;
    newNode*->leaf = true;
    newNode*->keys = (int*) malloc(sizeof(int) * (2 * T - 1));
    newNode*->children = (char**) malloc(sizeof(char*) * (2 * T));

    return id_str;
}
*/

void diskWrite(btreeNode *Node, int T){
    if(Node == NULL || Node->id == NULL){
        return;
    }

    char filename[128];
    snprintf(filename, sizeof(filename), "nodes\\node_%s.bin", Node->id);

    FILE *file = fopen(filename, "wb");
    if(file == NULL){
        printf("Erro ao abrir arquivo");
        return;
    }

    fwrite(&Node->n, sizeof(int), 1, file);
    fwrite(&Node->leaf, sizeof(bool), 1, file);
    fwrite(Node->keys, sizeof(int), 2 * T - 1, file);

    for(int i = 0; i < 2 * T ; i++){
        char buffer[BUFFERSIZE] = "";

        if(!Node->leaf && Node->children != NULL && Node->children[i] != NULL){
            strncpy(buffer, Node->children[i], BUFFERSIZE - 1);
        }

        fwrite(buffer, sizeof(char), BUFFERSIZE, file);

    }
    fclose(file);
}


btreeNode* diskRead(const char *Node_id, int T){
    if(Node_id == NULL){
        return NULL;
    }
    char filename[128];
    snprintf(filename, sizeof(filename), "nodes\\node_%s.bin", Node_id);

    FILE *file = fopen(filename, "rb");
    if(file == NULL){
        return NULL;
    }

    btreeNode *Node = (btreeNode*) malloc(sizeof(btreeNode));
    strncpy(Node->id,Node_id, BUFFERSIZE-1);

    fread(&Node->n, sizeof(int),1,file);
    fread(&Node->leaf, sizeof(bool),1,file);


    Node->keys = (int*) malloc(sizeof(int) * (2 * T -1));
    fread(Node->keys, sizeof(int), 2 * T -1, file);

    Node->children = (char**) malloc(sizeof(char*) * (2 * T));
    for(int i = 0; i < 2 * T; i++){
        Node->children[i] = (char*) malloc(sizeof(char) * BUFFERSIZE);
        fread(Node->children[i], sizeof(char), BUFFERSIZE, file);

    }
    fclose(file);
    return Node;
}




void saveMetadata(Metadata *meta) {
    FILE *file = fopen("meta\\meta.bin", "wb");
    if (file == NULL) {
        printf("Erro ao salvar metadados!\n");
        return;
    }
    fwrite(meta, sizeof(Metadata), 1, file);
    fclose(file);
}


bool loadMetadata(Metadata *meta) {
    FILE *file = fopen("meta\\meta.bin", "rb");
    if (file == NULL) {
        return false; // Arquivo não existe (primeira execução)
    }
    fread(meta, sizeof(Metadata), 1, file);
    fclose(file);
    return true;
}



