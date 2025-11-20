#include "tree.h"
#include "derivator.h"

static void OptimiseNode(TreeNode* node);

#define CNST_(val) \
    CreateNode(NODE_CONSTANT, (NodeValue){.constant = val})

#define ADD_(left, right) \
    CreateNode(NODE_OPERATION, (NodeValue){.operation = OP_ADD}, left, right)

#define SUB_(left, right) \
    CreateNode(NODE_OPERATION, (NodeValue){.operation = OP_SUB}, left, right)

#define MUL_(left, right) \
    CreateNode(NODE_OPERATION, (NodeValue){.operation = OP_MUL}, left, right)

#define DIV_(left, right) \
    CreateNode(NODE_OPERATION, (NodeValue){.operation = OP_DIV}, left, right)

#define dL\
    DerivateNode(node->left)

#define dR\
    DerivateNode(node->right)

#define cL\
    CopyNode(node->left)

#define cR\
    CopyNode(node->right)

Tree* DerivateTree(Tree* tree)
{
    Tree* new_tree = CreateTree();
    new_tree->root = DerivateNode(tree->root);
    
    return new_tree;
}

TreeNode* DerivateNode(TreeNode* node)
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

void OptimiseTree(Tree* tree)
{
    OptimiseNode(tree->root);
}

static void OptimiseNode(TreeNode* node)
{
    if(!node) return;

    if(node->type == NODE_CONSTANT || node->type == NODE_VARIABLE) return;

    // OPERATION WITH CONSTANTS
    if(node->left->type == NODE_CONSTANT && node->right->type == NODE_CONSTANT)
    {
        node->type = NODE_CONSTANT;

        switch (node->value.operation)
        {
            case OP_ADD:
            {
                node->value.constant = node->left->value.constant + node->right->value.constant;
                break;
            }
            case OP_SUB:
            {
                node->value.constant = node->left->value.constant - node->right->value.constant;
                break;
            }
            case OP_MUL:
            {
                node->value.constant = node->left->value.constant * node->right->value.constant;
                break;
            }
            case OP_DIV:
            {
                node->value.constant = node->left->value.constant / node->right->value.constant;
                break;
            }
            default:
                break;
        }

        NodeDestroy(node->left);
        NodeDestroy(node->right);
        node->left = NULL;
        node->right = NULL;
    }

    // MULTIPLICATION ON ZERO
    if(node->value.operation == OP_MUL && node->left && node->right)
    {
        if((node->left->type == NODE_CONSTANT && node->left->value.constant == 0)
        || (node->right->type == NODE_CONSTANT && node->right->value.constant == 0))
        {
            node->type = NODE_CONSTANT;
            node->value.constant = 0;

            NodeDestroy(node->left);
            NodeDestroy(node->right);
            node->left = NULL;
            node->right = NULL;
        }
    }

    // MULTIPLICATION ON ONE
    if(node->value.operation == OP_MUL && node->left && node->right)
    {
        if(node->left->type == NODE_CONSTANT && node->left->value.constant == 1)
        {
            node->type = node->right->type;
            node->value = node->right->value;

            TreeNode* old_left = node->left;
            TreeNode* old_right = node->right;

            node->left = CopyNode(old_right->left);
            node->right = CopyNode(old_right->right);

            NodeDestroy(old_left);
            NodeDestroy(old_right);
        }
        else if(node->right->type == NODE_CONSTANT && node->right->value.constant == 1)
        {
            node->type = node->left->type;
            node->value = node->left->value;

            TreeNode* old_left = node->left;
            TreeNode* old_right = node->right;

            node->left = CopyNode(old_left->left);
            node->right = CopyNode(old_left->right);

            NodeDestroy(old_left);
            NodeDestroy(old_right);
        }
    }

    // SUBSTRACTING ZERO
    if(node->value.operation == OP_SUB && node->left && node->right)
    {
        if(node->right->type == NODE_CONSTANT && node->right->value.constant == 0)
        {
            node->type = node->left->type;
            node->value = node->left->value;

            TreeNode* old_left = node->left;
            TreeNode* old_right = node->right;

            node->left = CopyNode(old_left->left);
            node->right = CopyNode(old_left->right);

            NodeDestroy(old_left);
            NodeDestroy(old_right);
        }
    }

    OptimiseNode(node->left);
    OptimiseNode(node->right);

    // ADDING ZERO

    if(node->value.operation == OP_ADD && node->left && node->right)
    {
        if(node->left->type == NODE_CONSTANT && node->left->value.constant == 0)
        {
            node->type = node->right->type;
            node->value = node->right->value;

            TreeNode* old_left = node->left;
            TreeNode* old_right = node->right;

            node->left = CopyNode(old_right->left);
            node->right = CopyNode(old_right->right);

            NodeDestroy(old_left);
            NodeDestroy(old_right);
        }
        else if(node->right->type == NODE_CONSTANT && node->right->value.constant == 0)
        {
            node->type = node->left->type;
            node->value = node->left->value;

            TreeNode* old_left = node->left;
            TreeNode* old_right = node->right;

            node->left = CopyNode(old_left->left);
            node->right = CopyNode(old_left->right);

            NodeDestroy(old_left);
            NodeDestroy(old_right);
        }
    }    
}