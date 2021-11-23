#ifndef DIFFERENTIATOR_H
#define DIFFERENTIATOR_H

#include "tree.h"
#include "include/onegin.h"

#define NUM(node_name) 				(node_name)->data.value.num
#define STR(node_name) 				(node_name)->data.value.str
#define TYPE(node_name)				(node_name)->data.data_type

#define NEW_CONST(val)				new_node(CONST, data_un_d(val), NULL, NULL, parent)
#define NEW_OPER(type, left, right, parent)	new_node(OPER, data_un_c(type), left, right, parent)

#define ADD(left, right)			new_node(OPER, data_un_c('+'), left, right, parent)
#define SUB(left, right)			new_node(OPER, data_un_c('-'), left, right, parent)
#define MUL(left, right)			new_node(OPER, data_un_c('*'), left, right, parent)
#define PWR(left, right)			new_node(OPER, data_un_c('^'), left, right, parent)
#define DIV(left, right)			new_node(OPER, data_un_c('/'), left, right, parent)

#define CR					copy_nodes(node->right, parent)
#define CL					copy_nodes(node->left, parent)
#define DR					DIFF(node->right, parent)
#define DL					DIFF(node->left, parent)

enum operators {
	OP_ADD = '+',
	OP_MUL = '*',
	OP_DIV = '/',
	OP_SUB = '-',
	OP_PWR = '^'
};
/*
 * 1)TODO pretty print
 * 2)TODO sin cos %s
 * 3)TODO parent in DIFF?
 * 4)TODO DIFF ret val
 * 5)TODO DIFF???
 * 6)TODO check errs in smal funcs
 */

int DiffInit(TNODE **node, textBuff *btext);
int DiffSave(TNODE *node);
int DiffPrint(TNODE *node, FILE *file = stdout);
TNODE *DIFF(TNODE *node, TNODE *parent = NULL);
int DiffProcess();

TNODE* new_node(int type, DATA val, TNODE *left, TNODE *right, TNODE *parent);
TNODE *copy_nodes(TNODE *src, TNODE *parent);


#endif // DIFFERENTIATOR_H
