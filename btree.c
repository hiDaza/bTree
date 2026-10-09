
#include "btree.h"


static char gBaseDir[256] = ".";

// Função para definir em qual pasta o disco deve ler/gravar
void setBaseDir(const char *path) {
    if (path != NULL && strlen(path) > 0) {
        strncpy(gBaseDir, path, sizeof(gBaseDir) - 1);
        gBaseDir[sizeof(gBaseDir) - 1] = '\0';
    }
}





btreeNode* createNode(int T, bool leaf, Metadata *meta){
    btreeNode* node = (btreeNode*) malloc(sizeof(btreeNode));

    numToBase62(meta->nextId++, node->id);

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
        strncpy(meta->rootId,newRoot->id, BUFFERSIZE-1); //com a criação do newRoot agora existe uma nova raiz que deve ser salva no metadata
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
    uint64_t raw_id;
char* allocateNode(btreeNode **newNode, int T, Metadata *meta) {

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

    char filename[512];
    snprintf(filename, sizeof(filename), "%s/nodes/node_%s.bin",gBaseDir,Node->id);

    FILE *file = fopen(filename, "wb");
    if(file == NULL){
        printf("Erro ao abrir arquivo");
        return;
    }

    fwrite(Node->id, sizeof(char), BUFFERSIZE,file);
    fwrite(&Node->leaf, sizeof(bool), 1, file);
    fwrite(&Node->n, sizeof(int), 1, file);
    fwrite(Node->keys, sizeof(int), 2 * T - 1, file);

    for(int i = 0; i < 2 * T; i++){
        fwrite(Node->children[i], sizeof(char),BUFFERSIZE,file);
    }

    for(int i = 0; i < 2 * T ; i++){
        char buffer[BUFFERSIZE] = "";

        if(!Node->leaf && Node->children != NULL && Node->children[i] != NULL){
            strncpy(buffer, Node->children[i], BUFFERSIZE - 1);
        }

        fwrite(buffer, sizeof(char), BUFFERSIZE, file);

    }
    fclose(file);
}


btreeNode* diskRead(const char *NodeId, int T){
    if(NodeId == NULL){
        return NULL;
    }
    char filename[128];
    snprintf(filename, sizeof(filename), "%s/nodes/node_%s.bin", gBaseDir,NodeId);

    FILE *file = fopen(filename, "rb");
    if(file == NULL){
        return NULL;
    }

    btreeNode *Node = (btreeNode*) malloc(sizeof(btreeNode));
    strncpy(Node->id,NodeId, BUFFERSIZE-1);

    fread(&Node->id, sizeof(char), BUFFERSIZE, file);
    fread(&Node->leaf, sizeof(bool),1,file);
    fread(&Node->n, sizeof(int),1,file);

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




bool saveMetadata(const Metadata *meta) {
    char filepath[512];
    snprintf(filepath, sizeof(filepath), "%s/meta/meta.bin", gBaseDir);

    FILE *file = fopen(filepath, "wb");
    if (!file) {
        return false;
    }

    fwrite(meta, sizeof(Metadata), 1, file);
    fclose(file);
    return true;
}

bool loadMetadata(Metadata *meta) {
    char filepath[512];
    snprintf(filepath, sizeof(filepath), "%s/meta/meta.bin", gBaseDir);

    FILE *file = fopen(filepath, "rb");
    if (!file) {
        return false;
    }

    fread(meta, sizeof(Metadata), 1, file);
    fclose(file);
    return true;
}


void printBtreeDFS(int T){
    Metadata meta;
    if(!loadMetadata(&meta)){
        return;
    }else{
        btreeNode *Node;
        Node = diskRead(meta.rootId,T);
        printBtreeRec(Node,T);
    }
}

void printBtreeRec(btreeNode *Node, int T){
printf("ID do Node: %s\n",Node->id);
    if(Node->leaf){
        printf("Eh Folha\n");
    }else{
    printf("Nao Eh Folha\n");
    }
    printf("Tamanho do Node: %d\n",Node->n);

    for(int i =0; i <= Node->n-1; i++){
        printf("Chaves da posicao [%d]: %d \n",i, Node->keys[i]);
        }
    btreeNode *nextNode;
    if(!Node->leaf){
            for(int j = 0; j <= Node->n ; j++){
            nextNode = diskRead(Node->children[j],T);
            printBtreeRec(nextNode,T);
        }
    }
}

void printBtreeBFS(int T){
    char queu [1000][BUFFERSIZE];
    int init = 0;
    int end = 0;
    int level = 0;
    Metadata meta;
    if(!loadMetadata(&meta)){
        return;
    }
    btreeNode *Node;
    Node = diskRead(meta.rootId,T);
    strncpy(queu[end],Node->id,BUFFERSIZE-1);
    end++;
    level++;
    while(init < end){
        printf(COR_AMARELO "Nivel %d: "COR_RESET, level);
        int nodeInTheLevel = end - init;

        for(int i =0; i < nodeInTheLevel; i++){
            char currentId[BUFFERSIZE];
            strncpy(currentId,queu[init],BUFFERSIZE-1);
            init++;

            btreeNode *currentNode = diskRead(currentId,T);

            printf(COR_AMARELO "ID <%s>: "COR_RESET, currentNode->id);

            printf(COR_VERDE "|" COR_RESET);
           // printf("COR_AMARELO Tamanho do Node: %d\n"COR_RESET,currentNode->n);

            for(int i =0; i <= currentNode->n-1; i++){
                printf(COR_VERDE "%d " COR_RESET, currentNode->keys[i]);
                if(i < currentNode->n-1){
                    printf(" ");
                    }
                }

                printf(COR_VERDE "|    " COR_RESET);
            if(!currentNode->leaf){
               // printf(COR_CIANO " (Filhos: ");
                for(int j = 0; j <= currentNode->n; j++){
                    strncpy(queu[end], currentNode->children[j],BUFFERSIZE-1);
                 //   printf("<%s>",currentNode->children[j]);
                    end++;
                }

            }
            freeNode(currentNode,T);
        }
        printf("\n\n");
        level++;
    }

}


void removeCLRS(int key, int T){
    Metadata meta;
    if(!loadMetadata(&meta)){
        return;
    }
    btreeNode *Node = diskRead(meta.rootId,T);
    searchResult search = searchBTree(Node,key,T);
    if(search.node == NULL){
        freeNode(Node,T);
        return;
    }
    //caso 1 é folha
    if(search.node->leaf){
        for(int i = search.index; i < search.node->n-1; i++){
            search.node->keys[i] = search.node->keys[i+1];
        }
        search.node->n--;
        diskWrite(search.node,T);
    }else{

        btreeNode *leftSon = diskRead(search.node->children[search.index],T);
           //caso 2 é nó interno sub caso (a)
        if(leftSon->n >= T){
            int newK = getPredecessor(leftSon,T);
            removeCLRS(newK,T);
            search.node->keys[search.index] = newK;
            freeNode(leftSon,T);
            diskWrite(search.node,T);
        }else{

            btreeNode *rightSon = diskRead(search.node->children[search.index+1],T);
            if(leftSon->n == T-1){
                //caso 2 sub caso (b)
                if(rightSon->n >= T){
                int newK = getSucessor(rightSon,T);
                removeCLRS(newK,T);
                search.node->keys[search.index] = newK;
                freeNode(rightSon,T);
                diskWrite(search.node,T);

                }else{
                    //caso 2 sub caso(c)
                    leftSon->keys[leftSon->n] = search.node->keys[search.index];
                    leftSon->n++;

                    for(int i = 0; i <= rightSon->n-1; i++){
                        leftSon->keys[leftSon->n] = rightSon->keys[i];
                        leftSon->children[leftSon->n] = rightSon->children[i];
                        leftSon->n++;
                    }
                    leftSon->children[leftSon->n] = rightSon->children[rightSon->n];

                    for(int i = search.index+1; i < search.node->n;i++){
                        search.node->keys[i-1] = search.node->keys[i];
                        search.node->children[i] = search.node->children[i+1];
                    }

                    search.node->n--;
                    freeNode(rightSon,T);
                    diskWrite(leftSon,T);
                    diskWrite(search.node,T);
                    removeCLRS(key,T);
                }

            }
        }


    }

}


int getPredecessor(btreeNode *Node,int T){
    int kLine = Node->keys[Node->n-1];
    if(!Node->leaf){
        btreeNode *son = diskRead(Node->children[Node->n],T);
        int result = getPredecessor(son,T);
        freeNode(son,T);

        return result;
    }else{
        return kLine;
    }
}

int getSucessor(btreeNode *Node, int T){
    int kLine = Node->keys[0];
    if(!Node->leaf){
        btreeNode *son = diskRead(Node->children[0],T);
        int result = getSucessor(son,T);
        freeNode(son,T);
        return result;
    }else{
        return kLine;
    }

}









