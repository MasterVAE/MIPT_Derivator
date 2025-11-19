#include <assert.h>

#include "tree.h"
#include "tree_logger.h"

static const char* const TREE_FILENAME = "files/tree.tr";

int main()
{
    Tree* tree = CreateTree();

    LoadTree(tree, TREE_FILENAME);

    TreeDotDump(tree);
    TreeTexDump(tree);

    TreeDestroy(tree);
    return 0;
}