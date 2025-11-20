#include "tree.h"
#include "derivator.h"

#define CNST_(val) \
    CreateNode(NODE_CONSTANT, (NodeValue){.constant = val})

#define ADD_(left, right) \
    CreateNode(NODE_OPERATION, (NodeValue){.operation = OP_ADD}, left, right, parent)

#define SUB_(left, right) \
    CreateNode(NODE_OPERATION, (NodeValue){.operation = OP_SUB}, left, right, parent)

#define MUL_(left, right) \
    CreateNode(NODE_OPERATION, (NodeValue){.operation = OP_MUL}, left, right, parent)

#define DIV_(left, right) \
    CreateNode(NODE_OPERATION, (NodeValue){.operation = OP_DIV}, left, right, parent)

#define dL\
    DerivateNode(node->left, node)

#define dR\
    DerivateNode(node->right, node)

#define cL\
    CopyNode(node->left)

#define cR\
    CopyNode(node->right)

Tree* DerivateTree(Tree* tree)
{
    Tree* new_tree = CreateTree();
    new_tree->root = DerivateNode(tree->root, NULL);
    
    return new_tree;
}

TreeNode* DerivateNode(TreeNode* node, TreeNode* parent)
{
    if(node->type == NODE_CONSTANT)
    {
        return CNST_(0);
    }
    
    if(node->type == NODE_VARIABLE)
    {
        if(node->value.variable == 'X')
        {
            return CNST_(1);
        }
        
        return CNST_(0);
    }

    switch (node->value.operation)
    {
        case OP_ADD:
        {
            return ADD_(dL, dR);
        }
        case OP_SUB:
        {
            return SUB_(dL, dR);
        }
        case OP_MUL:
        {
            return ADD_(MUL_(cR, dL), MUL_(cL, dR));
        }
        case OP_DIV:
        {
            return DIV_(SUB_(MUL_(cR, dL), MUL_(cL, dR)), MUL_(cR, cR));
        }
        default:
            return NULL;
    }
}