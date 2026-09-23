#include "btree.h"

int main()
{
    int T = 3;
    bool leaf = true;
    btreeNode *b;
    b = createNode(T,leaf);

    printf("%d",b->keys[2]);

}

