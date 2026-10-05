#include "btree.h"

int main() {
    int T = 3;
    Metadata meta;

    if (!loadMetadata(&meta)) {
        meta.next_id = 0;
        meta.root_id[0] = '\0';
        saveMetadata(&meta);
    }

    // Vetor ou Laço com 60 valores para gerar ~12 nós
    for (int i = 1; i <= 60; i++) {
        // Exemplo inserindo chaves de 10 em 10 (10, 20, 30...) ou aleatórias
        insertCLRSNode(&meta, i * 10, T);
    }

    // Salva os metadados finais com o próximo ID e a raiz atualizada
    saveMetadata(&meta);

    printf("Inserção concluída! Raiz final: %s, Próximo ID: %lu\n",
            meta.root_id, meta.next_id);

    return 0;
}
