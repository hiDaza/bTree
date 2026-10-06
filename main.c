
#include "btree.h"

void prepararPastasDoTeste(const char *base_dir) {
    char cmd[512];
    #ifdef _WIN32
        snprintf(cmd, sizeof(cmd), "if not exist \"%s\\nodes\" mkdir \"%s\\nodes\" && if not exist \"%s\\meta\" mkdir \"%s\\meta\"", base_dir, base_dir);
    #else
        snprintf(cmd, sizeof(cmd), "mkdir -p \"%s/nodes\" \"%s/meta\"", base_dir, base_dir);
    #endif
    system(cmd);
}

// Imprime conteudo do nó - usando por enquanto até criar o print da árvore
void inspecionarNo(const char *node_id, int T) {
    if (node_id == NULL || strlen(node_id) == 0) return;

    btreeNode *no = diskRead(node_id, T);
    if (no == NULL) {
        printf("   [ERRO] Nao foi possivel ler o no %s do disco.\n", node_id);
        return;
    }

    printf("\n   === NO (%s) ===\n", no->id);
    printf("   EH Folha?: %s | Qtd Chaves (n): %d\n", no->leaf ? "Sim" : "Nao", no->n);
    printf("   Chaves: ");
    for (int i = 0; i < no->n; i++) {
        printf("[%d] ", no->keys[i]);
    }
    printf("\n");

    if (!no->leaf) {
        printf("   IDs dos Filhos: ");
        for (int i = 0; i <= no->n; i++) {
            printf("<%s> ", no->children[i]);
        }
        printf("\n");
    }
    printf("   =================\n");

    freeNode(no, T);
}


// TESTE 1: Inserção squencial crescente
void rodarTeste1_Sequencial(int T) {
    const char *dir = "testes/teste_01";
    printf("\n==================================================\n");
    printf("  TESTE 1: Insercao Sequencial Crescente (10..600)\n");
    printf("==================================================\n");

    setBaseDir(dir);
    prepararPastasDoTeste(dir);

    Metadata meta;
    if (!loadMetadata(&meta)) {
        meta.next_id = 0;
        meta.root_id[0] = '\0';
        saveMetadata(&meta);
    }

    if (strlen(meta.root_id) == 0) {
        printf("[INFO] Populando Teste 1 do zero...\n");
        for (int i = 1; i <= 60; i++) {
            int chave = i * 10;
            if (strlen(meta.root_id) == 0) {
                btreeNode *root = createNode(T, true, &meta);
                root->keys[0] = chave;
                root->n = 1;
                strncpy(meta.root_id, root->id, BUFFERSIZE - 1);
                diskWrite(root, T);
                freeNode(root, T);
            } else {
                btreeNode *root = diskRead(meta.root_id, T);
                insertCLRSNode(root, chave, T, &meta);
            }
        }
        saveMetadata(&meta);
    } else {
        printf("[INFO] Teste 1 ja existe no disco. Exibindo dados persistidos.\n");
    }

    printf("[RESULTADO] Raiz Atual: %s | Proximo ID: %lu\n", meta.root_id, meta.next_id);
    inspecionarNo(meta.root_id, T);
}

// TESTE 2: Inserção sequencial decrescente
void rodarTeste2_Decrescente(int T) {
    const char *dir = "testes/teste_02";
    printf("\n==================================================\n");
    printf("  TESTE 2: Insercao Sequencial Decrescente (600..10)\n");
    printf("==================================================\n");

    setBaseDir(dir);
    prepararPastasDoTeste(dir);

    Metadata meta;
    if (!loadMetadata(&meta)) {
        meta.next_id = 0;
        meta.root_id[0] = '\0';
        saveMetadata(&meta);
    }

    if (strlen(meta.root_id) == 0) {
        printf("[INFO] Populando Teste 2 do zero...\n");
        for (int i = 60; i >= 1; i--) {
            int chave = i * 10;
            if (strlen(meta.root_id) == 0) {
                btreeNode *root = createNode(T, true, &meta);
                root->keys[0] = chave;
                root->n = 1;
                strncpy(meta.root_id, root->id, BUFFERSIZE - 1);
                diskWrite(root, T);
                freeNode(root, T);
            } else {
                btreeNode *root = diskRead(meta.root_id, T);
                insertCLRSNode(root, chave, T, &meta);
            }
        }
        saveMetadata(&meta);
    } else {
        printf("[INFO] Teste 2 ja existe no disco. Exibindo dados persistidos.\n");
    }

    printf("[RESULTADO] Raiz Atual: %s | Proximo ID: %lu\n", meta.root_id, meta.next_id);
    inspecionarNo(meta.root_id, T);
}

// TESTE 3: Inserção alternada

void rodarTeste3_Desordenado(int T) {
    const char *dir = "testes/teste_03";
    printf("\n==================================================\n");
    printf("  TESTE 3: Insercaoo Alternada\n");
    printf("==================================================\n");

    setBaseDir(dir);
    prepararPastasDoTeste(dir);

    Metadata meta;
    if (!loadMetadata(&meta)) {
        meta.next_id = 0;
        meta.root_id[0] = '\0';
        saveMetadata(&meta);
    }

    if (strlen(meta.root_id) == 0) {
        printf("[INFO] Populando Teste 3 do zero...\n");
        int chaves[] = {150, 50, 200, 20, 80, 300, 10, 40, 90, 110, 120, 130, 250, 270, 280};
        int total = sizeof(chaves) / sizeof(chaves[0]);

        for (int i = 0; i < total; i++) {
            if (strlen(meta.root_id) == 0) {
                btreeNode *root = createNode(T, true, &meta);
                root->keys[0] = chaves[i];
                root->n = 1;
                strncpy(meta.root_id, root->id, BUFFERSIZE - 1);
                diskWrite(root, T);
                freeNode(root, T);
            } else {
                btreeNode *root = diskRead(meta.root_id, T);
                insertCLRSNode(root, chaves[i], T, &meta);
            }
        }
        saveMetadata(&meta);
    } else {
        printf("[INFO] Teste 3 ja existe no disco. Exibindo dados persistidos.\n");
    }

    printf("[RESULTADO] Raiz Atual: %s | Proximo ID: %lu\n", meta.root_id, meta.next_id);
    inspecionarNo(meta.root_id, T);
}

//TESTE 4 150 elementos
void rodarTeste4_Carga(int T) {
    const char *dir = "testes/teste_04";
    printf("\n==================================================\n");
    printf("  TESTE 4: Teste de Carga (150 Elementos)\n");
    printf("==================================================\n");

    setBaseDir(dir);
    prepararPastasDoTeste(dir);

    Metadata meta;
    if (!loadMetadata(&meta)) {
        meta.next_id = 0;
        meta.root_id[0] = '\0';
        saveMetadata(&meta);
    }

    if (strlen(meta.root_id) == 0) {
        printf("[INFO] Populando Teste 4 do zero...\n");
        for (int i = 1; i <= 150; i++) {
            int chave = i * 5;
            if (strlen(meta.root_id) == 0) {
                btreeNode *root = createNode(T, true, &meta);
                root->keys[0] = chave;
                root->n = 1;
                strncpy(meta.root_id, root->id, BUFFERSIZE - 1);
                diskWrite(root, T);
                freeNode(root, T);
            } else {
                btreeNode *root = diskRead(meta.root_id, T);
                insertCLRSNode(root, chave, T, &meta);
            }
        }
        saveMetadata(&meta);
    } else {
        printf("[INFO] Teste 4 ja existe no disco. Exibindo dados persistidos.\n");
    }

    printf("[RESULTADO] Raiz Atual: %s | Proximo ID: %lu\n", meta.root_id, meta.next_id);
    inspecionarNo(meta.root_id, T);
}


// TESTE 5 - busca
void rodarTeste5_Busca(int T) {
    const char *dir = "testes/teste_01"; // Consulta a massa de dados gerada no Teste 1
    printf("\n==================================================\n");
    printf("  TESTE 5: Busca de Chave no Teste 1 (Apenas Leitura)\n");
    printf("==================================================\n");

    setBaseDir(dir);

    Metadata meta;
    if (!loadMetadata(&meta) || strlen(meta.root_id) == 0) {
        printf("[ERRO] Execute o Teste 1 primeiro para gerar os dados no disco!\n");
        return;
    }

    btreeNode *root = diskRead(meta.root_id, T);
    int chave_busca = 250;

    printf("[INFO] Realizando searchBTree da chave %d...\n", chave_busca);
    searchResult res = searchBTree(root, chave_busca, T);

    if (res.node != NULL) {
        printf("[SUCESSO] Chave %d encontrada no No ID: %s (Índice: %d)\n",
               chave_busca, res.node->id, res.index);

        if (res.node != root) {
            freeNode(res.node, T);
        }
    } else {
        printf("[FALHA] Chave %d nao encontrada na arvore.\n", chave_busca);
    }

    freeNode(root, T);
}

int main() {
    int T = 3;
    rodarTeste1_Sequencial(T);
    rodarTeste2_Decrescente(T);
    rodarTeste3_Desordenado(T);
    rodarTeste4_Carga(T);
    rodarTeste5_Busca(T);

    return 0;
}
