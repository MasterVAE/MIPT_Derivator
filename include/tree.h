#ifndef TREE_H_
#define TREE_H_

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
};

struct Tree
{
    TreeNode* root; 
};


Tree* CreateTree();
void TreeDestroy(Tree* tree);
TreeNode* CreateNode(NodeType type, NodeValue value);
void LoadTree(Tree* tree, const char* filename);

#endif // TREE_H_