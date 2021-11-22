#ifndef DIFFERENTIATOR_H
#define DIFFERENTIATOR_H

#include "tree.h"
#include "include/onegin.h"

#define NUM(node_name) 		(node_name)->data.value.num
#define STR(node_name) 		(node_name)->data.value.str

#define NEW_NODE(type, val, left, right, parent) 
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
 *
 */
int DiffInit(TNODE **node, textBuff *btext);
int DiffSave(TNODE *node);
int DiffPrint(TNODE *node, FILE *file = stdout);
int DIFF(TNODE *node, int type);
int DiffProcess();

#endif // DIFFERENTIATOR_H
