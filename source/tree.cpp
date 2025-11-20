#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include <assert.h>

#include "tree.h"

static TreeNode* LoadNode(char** buffer, TreeNode* parent = NULL);
static void NodeDestroy(TreeNode* node);
static size_t FileLength(FILE* file);
static size_t Scan(const char* source, char* dest);

Tree* CreateTree()
{
    Tree* tree = (Tree*)calloc(1, sizeof(Tree));
    tree->root = NULL;

    return tree;
}

void TreeDestroy(Tree* tree)
{
    if(!tree) return;

    NodeDestroy(tree->root);
    free(tree);
}

static void NodeDestroy(TreeNode* node)
{
    if(!node) return;

    NodeDestroy(node->left);
    NodeDestroy(node->right);

    free(node);
}

TreeNode* CreateNode(NodeType type, NodeValue value, 
                                    TreeNode* left, TreeNode* right, TreeNode* parent)
{
    TreeNode* node = (TreeNode*)calloc(1, sizeof(TreeNode));
    node->type = type;
    node->value = value;

    node->left = left;
    node->right = right;

    node->parent = parent;

    return node;
}

void LoadTree(Tree* tree, const char* filename)
{
    assert(tree);
    assert(filename);

    FILE* file = fopen(filename, "r");
    if(!file) return;

    size_t filesize = FileLength(file);

    char* buffer = (char*)calloc(filesize + 1, sizeof(char));
    char* buffer_start = buffer;

    if(!buffer)
    {
        fclose(file);
        return;
    }

    fread(buffer, sizeof(char), filesize, file);
    buffer[filesize] = '\0';

    fclose(file);
    
    tree->root = LoadNode(&buffer);

    free(buffer_start);
}

static TreeNode* LoadNode(char** buffer, TreeNode* parent)
{
    assert(buffer);
    assert(*buffer);

    (*buffer)++;
    char c = *buffer[0];
    switch (c)
    {
        case ')':
            return NULL;
        case 'O':
        {
            (*buffer)++;
            char operation = *buffer[0];

            TreeNode* node;
            NodeValue value;
            switch (operation)
            {
                case '+':
                {
                    value.operation = OP_ADD;
                    break;
                }
                case '-':
                {
                    value.operation = OP_SUB;
                    break;
                }
                case '*':
                {
                    value.operation = OP_MUL;
                    break;
                }
                case '/':
                {
                    value.operation = OP_DIV;
                    break;
                }
                default:
                    return NULL; 
            }

            node = CreateNode(NODE_OPERATION, value, NULL, NULL, parent);

            (*buffer)++;
            if((*buffer)[0] == ')') return node;

            node->left = LoadNode(buffer, node);       
            node->right = LoadNode(buffer, node);

            (*buffer)++;
            return node;
        }
        case 'V':
        {
            (*buffer)++;
            char variable = *buffer[0];

            TreeNode* node = CreateNode(NODE_VARIABLE, (NodeValue){.variable = variable}, 
                                                        NULL, NULL, parent);

            *buffer += 2;

            return node;
        }
        case 'C':
        {
            (*buffer)++;

            char number_str[100] = {}; // FIXME bebra
            size_t scanned_count = Scan(*buffer, number_str);

            NodeValue value;
            sscanf(number_str, "%lg", &value.constant);

            TreeNode* node = CreateNode(NODE_CONSTANT, value, NULL, NULL, parent);

            *buffer += scanned_count;

            (*buffer)++;
            return node;
        }
        default:
            return NULL;
    }
}

static size_t FileLength(FILE* file)
{
    assert(file);

    fseek(file, 0, SEEK_END);
    size_t filesize = (size_t)ftell(file);
    fseek(file, 0, SEEK_SET);

    return filesize;
}

static size_t Scan(const char* source, char* dest)
{
    char c;
    size_t i = 0;
    while((c = source[i]) != '(' && c != ')')
    {
        dest[i] = c;
        i++;
    }

    dest[i] = '\0';

    return i;
}

TreeNode* CopyNode(TreeNode* node)
{
    if(!node) return NULL;

    return CreateNode(node->type, node->value, CopyNode(node->left), CopyNode(node->right));
}