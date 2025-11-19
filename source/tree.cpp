#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include <assert.h>

#include "tree.h"

static TreeNode* LoadNode(char** buffer);
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

TreeNode* CreateNode(NodeType type, NodeValue value)
{
    TreeNode* node = (TreeNode*)calloc(1, sizeof(TreeNode));
    node->type = type;
    node->value = value;

    node->left = NULL;
    node->right = NULL;

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

static TreeNode* LoadNode(char** buffer)
{
    assert(buffer);

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

            switch (operation)
            {
                case '+':
                {
                    NodeValue value;
                    value.operation = OP_ADD;
                    node = CreateNode(NODE_OPERATION, value);

                    break;
                }
                case '-':
                {
                    NodeValue value;
                    value.operation = OP_SUB;
                    node = CreateNode(NODE_OPERATION, value);

                    break;
                }
                case '*':
                {
                    NodeValue value;
                    value.operation = OP_MUL;
                    node = CreateNode(NODE_OPERATION, value);

                    break;
                }
                case '/':
                {
                    NodeValue value;
                    value.operation = OP_DIV;
                    node = CreateNode(NODE_OPERATION, value);

                    break;
                }
                default:
                    return NULL;
            }

            (*buffer)++;
            if((*buffer)[0] == ')') return node;

            node->left = LoadNode(buffer);

            *buffer = strchr(*buffer + 1, '(');

            node->right = LoadNode(buffer);

            (*buffer)++;
            return node;
        }
        case 'V':
        {
            (*buffer)++;
            char variable = *buffer[0];

            NodeValue value;
            value.variable = variable;
            TreeNode* node = CreateNode(NODE_VARIABLE, value);

            (*buffer)++;
            if((*buffer)[0] == ')') return node;

            node->left = LoadNode(buffer);

            *buffer = strchr(*buffer + 1, '(');

            node->right = LoadNode(buffer);

            (*buffer)++;
            return node;
        }
        case 'C':
        {
            (*buffer)++;

            char number_str[100] = {}; // FIXME bebra
            size_t scanned_count = Scan(*buffer, number_str);

            NodeValue value;
            sscanf(number_str, "%lg", &value.constant);

            TreeNode* node = CreateNode(NODE_CONSTANT, value);

            *buffer += scanned_count;
            if((*buffer)[0] == ')') return node;

            node->left = LoadNode(buffer);

            *buffer = strchr(*buffer + 1, '(');

            node->right = LoadNode(buffer);

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