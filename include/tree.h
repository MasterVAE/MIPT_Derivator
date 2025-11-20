#ifndef TREE_H_
#define TREE_H_

#include <stdlib.h>

enum NodeType
{
    NODE_OPERATION,
    NODE_VARIABLE,
    NODE_CONSTANT
};

enum Operation
{
    OP_MUL,
    OP_ADD,
    OP_DIV,
    OP_SUB
};

union NodeValue
{
    char variable;
    double constant;
    Operation operation;
};

struct TreeNode
{
    NodeType type;
    NodeValue value;

    TreeNode* left;
    TreeNode* right;

    TreeNode* parent;
};

struct Tree
{
    TreeNode* root; 
};


Tree* CreateTree();
void TreeDestroy(Tree* tree);
void NodeDestroy(TreeNode* node);
TreeNode* CreateNode(NodeType type, NodeValue value, 
                     TreeNode* left = NULL, TreeNode* right = NULL, TreeNode* parent = NULL);
void LoadTree(Tree* tree, const char* filename);
TreeNode* CopyNode(TreeNode* node);
void SetParents(Tree* tree);

#endif // TREE_H_