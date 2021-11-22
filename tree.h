#ifndef TREE_H
#define TREE_H

#include "include/error.h"
#include "config.h"
#include "include/onegin.h"

#include <cstring>
#include <stdio.h>
#include <cassert>
#include <cstdlib>
#include <typeinfo>
#include <stdint.h>
#include <inttypes.h>

#define  $ 			printf("\t\t\t---ON LINE %d IN FUNCTION %s---\n", __LINE__, __func__);
//#define $ ;
#define SET_ERR(errtype)        return ERRNUM = errtype;

#define CHECK_(what, code)      if (what) SET_ERR (code)

#define CHECK_BREAK(what, code)	if (what) {SET_ERR(code); break;}

#define ERRNUM_CHECK(ret_val)   						\
	{									\
		if(ERRNUM) {							\
			perror(errmsg(ERRNUM));					\
			return ret_val;						\
		}								\
	}

#define TREE_CHECK(node, ret_val)						\
	{									\
		if (TreeCheck(node)) {						\
			perror(errmsg(ERRNUM));					\
			TREE_EXIT_ERR(ret_val);					\
		}								\
	}			

#if DEBUG == 1                 
#define TREE_EXIT_ERR(ret) assert(!"ok");

#else
#define TREE_EXIT_ERR(ret) return ret;
#endif

/*
 * 2)TODO validation
 * 3)TODO int tree dump 
 */
enum DATA_TYPES {
	CONST = 0,
	OPER  = 1,
	VAR   = 2
};
union DATA {
	double num;
	char str; // TODO char * ???
};

struct diff_data {
	DATA value;
	int data_type;
};

typedef diff_data tval_t;

struct TNODE {
	tval_t data;
	TNODE *left;
	TNODE *right;
	TNODE *parent;
#ifdef VALIDATE_TREE_INTEGRY
	int visit_flag;
#endif
};

enum SUBTREE_SIDE {
	LEFT  = 0,
	RIGHT = 1
};

int TreeInsert(TNODE *parent, int side, tval_t data);

int TreeCtor(TNODE **root, tval_t val = {}, TNODE *parent = NULL);
int TreeDtor(TNODE *node);
int TreePrintNode(TNODE *node);
int TreeCopy(TNODE *src, TNODE **dst, TNODE *parent);
void TreeDump(TNODE *node);

diff_data set_diff_data(int data_type = 0, DATA val = {});
DATA data_un(double num = 0);
DATA data_un(char str = 0);

void TreeDotDump(TNODE *node);
void VisitPrint(TNODE *node, FILE *fout = stdout);
int TreeCheck(TNODE *node);
#endif // TREE_H
