#ifndef DIFFERENTIATOR_H
#define DIFFERENTIATOR_H

#include "tree.h"
#include "include/onegin.h"

#include <math.h>

#define NUM(node_name) 				(node_name)->data.value.num
#define STR(node_name) 				(node_name)->data.value.str
#define TYPE(node_name)				(node_name)->data.data_type


#define NEW_CONST(val)				new_node(CONST, data_un_d(val), NULL, NULL, parent)
#define NEW_OPER(type, left, right, parent)	new_node(OPER, data_un_c(type), left, right, parent)

#define ADD(left, right)			new_node(OPER,  data_un_c('+'), left, right, parent)
#define SUB(left, right)			new_node(OPER,  data_un_c('-'), left, right, parent)
#define MUL(left, right)			new_node(OPER,  data_un_c('*'), left, right, parent)
#define PWR(left, right)			new_node(OPER,  data_un_c('^'), left, right, parent)
#define DIV(left, right)			new_node(OPER,  data_un_c('/'), left, right, parent)
#define SIN(left)				new_node(UOPER, data_un_c('S'), left, NULL,  NULL)
#define COS(left)				new_node(UOPER, data_un_c('C'), left, NULL,  NULL)

#define CR					copy_nodes(node->right, parent)
#define CL					copy_nodes(node->left, parent)
#define DR					DIFF(node->right, parent)
#define DL					DIFF(node->left, parent)

#define IS_NUM(node, val)			((TYPE(node) == CONST) && (cmp_double(val, NUM(node)) == 0))
#define REPLACE_CONST(node, op)			replace_const(node, (NUM(node->left) op NUM(node->right)))
enum operators {
	OP_ADD = '+',
	OP_MUL = '*',
	OP_DIV = '/',
	OP_SUB = '-',
	OP_PWR = '^'
};

enum unary_operators {
	UOP_SIN = 'S',
	UOP_COS = 'C'
};
const double ACCURACY = 10e-6;

/*
 * 1)TODO pretty print
 * 2)TODO sin cos %s
 */

int DiffInit(TNODE **node, textBuff *btext);
int DiffSave(TNODE *node);
int DiffPrint(TNODE *node, FILE *file = stdout);
int DiffTreeSimplify(TNODE *root);
TNODE *DIFF(TNODE *node, TNODE *parent = NULL);
int DiffProcess();

TNODE *new_node(int type, DATA val, TNODE *left, TNODE *right, TNODE *parent);
TNODE *copy_nodes(TNODE *src, TNODE *parent);


#endif // DIFFERENTIATOR_H
