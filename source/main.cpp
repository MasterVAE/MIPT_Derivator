#include <assert.h>

#include "tree.h"
#include "tree_logger.h"
#include "derivator.h"

static const char* const TREE_FILENAME = "files/tree.tr";

static const char* const TREE_IMAGE_1 = "files/tree1.png";
static const char* const TREE_IMAGE_2 = "files/tree2.png";

static const char* const TREE_TEX_1 = "files/tex1.tx";
static const char* const TREE_TEX_2 = "files/tex2.tx";

int main()
{
    Tree* tree = CreateTree();

    LoadTree(tree, TREE_FILENAME);

    TreeDotDump(tree, TREE_IMAGE_1);
    TreeTexDump(tree, TREE_TEX_1);

    Tree* d_tree = DerivateTree(tree);

    TreeDotDump(d_tree, TREE_IMAGE_2);
    TreeTexDump(d_tree, TREE_TEX_2);

    TreeDestroy(d_tree);
    TreeDestroy(tree);
    return 0;
}