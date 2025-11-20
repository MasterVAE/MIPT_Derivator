#include <stdlib.h>
#include <stdio.h>
#include <assert.h>

#include "tree.h"
#include "tree_logger.h"

static const char* const DOT_FILENAME = "files/dot.dt";
static const char* const TEX_FILENAME = "files/tex.tx";

static void TreeDotDumpStart(FILE* file);
static void TreeDotDumpEnd(FILE* file);

static void DotPrintNode(TreeNode* node, FILE* file, int rank);
static void TexPrintNode(TreeNode* node, FILE* file);
static void DotPrintValue(TreeNode* node, FILE* file);

static const size_t BUFFER_SIZE = 128;

void TreeDotDump(Tree* tree, const char* tree_image)
{
    assert(tree);

    FILE* file = fopen(DOT_FILENAME, "w+");

    TreeDotDumpStart(file);
    DotPrintNode(tree->root, file, 0);
    TreeDotDumpEnd(file);

    fclose(file);

    char command[BUFFER_SIZE] = {};
    snprintf(command, BUFFER_SIZE, "dot -Tpng %s -o %s", DOT_FILENAME, tree_image);
    system(command);
}


static void DotPrintNode(TreeNode* node, FILE* file, int rank)
{
    assert(node);
    assert(file);

    fprintf(file, "subgraph cluster_%p                                     \n", node);
    fprintf(file, "{                                                       \n");
    fprintf(file, "\tstyle=filled;                                         \n");
    fprintf(file, "\trank=%d;                                              \n", rank);
    fprintf(file, "\tfillcolor=lavender;                                   \n");
    fprintf(file, "\t\"%p_address\"[label=\"%p\",fillcolor=\"lavender\"];  \n", node, node);
    fprintf(file, "\t\"%p_parent\"[label=\"prnt: %p\","    
                                                "fillcolor=\"lavender\"];  \n", node, node->parent);

    DotPrintValue(node, file);

    fprintf(file, "\tsubgraph \"%p_bottom\"                                \n", node);
    fprintf(file, "\t{                                                     \n");
    fprintf(file, "\t\t style=invis;                                       \n");
    if(node->left) {
    fprintf(file, "\t\t\"%p_left\"[label=\"%p\",fillcolor=\"coral\"];      \n",node,  node->left);}

    if(node->right) {
    fprintf(file, "\t\t\"%p_right\"[label=\"%p\",fillcolor=\"lightgreen\"];\n", node, node->right);}
    
    fprintf(file, "\t}                                                     \n");
    fprintf(file, "\t\"%p_address\"->\"%p_parent\"[style=invis,weight=10]; \n", node, node);
    fprintf(file, "\t\"%p_parent\"->\"%p_value\"[style=invis,weight=10];   \n", node, node);
    
    if(node->left) {
    fprintf(file, "\t\"%p_value\"->\"%p_left\"[style=invis,weight=10];     \n", node, node);}

    if(node->right) {
    fprintf(file, "\t\"%p_value\"->\"%p_right\"[style=invis,weight=10];    \n", node , node);}
    
    fprintf(file, "}                                                       \n");

    if (node->left)
    {
        fprintf(file, "\"%p_left\" -> \"%p_address\";\n", node, node->left);
        DotPrintNode(node->left, file, rank + 1);
    }
    if (node->right)
    {
        fprintf(file, "\"%p_right\" -> \"%p_address\";\n", node, node->right);
        DotPrintNode(node->right, file, rank + 1);
    }
}

static void TreeDotDumpStart(FILE* file)
{
    assert(file);

    fprintf(file, "digraph G {"
                  "\t node [shape=plaintext, style=\"filled\"];\n"
                  "\t rankdir=TB;\n");
}

static void TreeDotDumpEnd(FILE* file)
{
    assert(file);

    fprintf(file, "}\n");
}


static void DotPrintValue(TreeNode* node, FILE* file)
{
    assert(node);
    assert(file);

    if(node->type == NODE_OPERATION)
    {
        switch (node->value.operation)
        {
            case OP_ADD:
                fprintf(file, "\t\"%p_value\"[label=\"+\",fillcolor=\"lavender\"];  \n", node);
                break;
            case OP_SUB:
                fprintf(file, "\t\"%p_value\"[label=\"-\",fillcolor=\"lavender\"];  \n", node);
                break;
            case OP_MUL:
                fprintf(file, "\t\"%p_value\"[label=\"*\",fillcolor=\"lavender\"];  \n", node);
                break;
            case OP_DIV:
                fprintf(file, "\t\"%p_value\"[label=\"/\",fillcolor=\"lavender\"];  \n", node);
                break;
            default:
                break;
        }   
    }
    else if(node->type == NODE_VARIABLE)
    {
        fprintf(file, "\t\"%p_value\"[label=\"%c\",fillcolor=\"lavender\"];  \n", node, node->value.variable);
    }
    else if(node->type == NODE_CONSTANT)
    {
        fprintf(file, "\t\"%p_value\"[label=\"%lg\",fillcolor=\"lavender\"];  \n", node, node->value.constant);
    }
}


void TreeTexDump(Tree* tree, const char* tree_tex)
{
    assert(tree);

    FILE* file = fopen(tree_tex, "w+");
    assert(file);

    TexPrintNode(tree->root, file);
    fprintf(file, "\\\\");
}

static void TexPrintNode(TreeNode* node, FILE* file)
{
    assert(node);
    assert(file);

    if(node->type == NODE_CONSTANT)
    {
        fprintf(file, "%lg", node->value.constant);
    }
    else if(node->type == NODE_VARIABLE)
    {
        fprintf(file, "%c", node->value.variable);
    }
    else
    {
        switch (node->value.operation)
        {
            case OP_ADD:
            {
                fprintf(file, "(");
                TexPrintNode(node->left, file);
                fprintf(file, "+");
                TexPrintNode(node->right, file);
                fprintf(file, ")");
                break;
            }
            case OP_MUL:
            {
                fprintf(file, "(");
                TexPrintNode(node->left, file);
                fprintf(file, "*");
                TexPrintNode(node->right, file);
                fprintf(file, ")");
                break;
            }
            case OP_SUB:
            {
                fprintf(file, "(");
                TexPrintNode(node->left, file);
                fprintf(file, "-");
                TexPrintNode(node->right, file);
                fprintf(file, ")");
                break;
            }
            case OP_DIV:
            {
                fprintf(file, "\\frac{");
                TexPrintNode(node->left, file);
                fprintf(file, "}{");
                TexPrintNode(node->right, file);
                fprintf(file, "}");
                break;
            }
            default:
                break;
        }
    }
}