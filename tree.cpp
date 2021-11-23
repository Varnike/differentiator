#include "tree.h"

static void visit_check(TNODE *node, int flagval = 0);

static int TreeTraverseIn(TNODE *root);
static int TreeTraverseDelete(TNODE *root);
static int tree_copy(TNODE *src, TNODE **dst, TNODE *parent);

int _tree_check(TNODE *node);

FILE *file = NULL;

int TreeCtor(TNODE **root, tval_t val, TNODE *parent)
{
	CHECK_(*root != NULL, TREE_BAD_CTOR_ROOT);
	
	*root = (TNODE*)calloc(1,sizeof(TNODE));

	if (*root == NULL)
		return ERRNUM = CALLOC_ERR;

		
	set_node(*root, val, parent);
	
	return 0;
}

int TreeInsert(TNODE *parent, int side, tval_t data)
{
	TREE_CHECK(parent, ERRNUM);

	TNODE *node = (TNODE*)calloc(1,sizeof(TNODE));

	if (node == NULL) {
		ERRNUM = CALLOC_ERR;
		goto err_free_buff;
	}
	
	set_node(node, data);

	if (side == LEFT) {
		if (parent->left != NULL) {
			ERRNUM = TREE_BUSY_PARENT;
			goto err_free_buff;
		}
		parent->left  = node;
	} else {
		if (parent->right != NULL) {
			ERRNUM = TREE_BUSY_PARENT;
			goto err_free_buff;
		}

		parent->right = node;
	}
	
	TREE_CHECK(parent, ERRNUM);
	return 0;

err_free_buff:
	free(node);
	return ERRNUM;
}

int TreeCopy(TNODE *src, TNODE **dst, TNODE *parent)
{
	TREE_CHECK(src, ERRNUM);
	
	tree_copy(src, dst, parent);

	return ERRNUM;
}

int TreeDtor(TNODE *node) 
{
	TREE_CHECK(node, ERRNUM);
	
	TreeTraverseDelete(node);
	ERRNUM_CHECK(ERRNUM);	

	return 0;
}

void TreeDump(TNODE *root)
{
	TREE_CHECK(root, ERRNUM);//TODO

	file = fopen("dump/dump.dot", "w");
	assert(file);

	if (file == NULL) {
		ERRNUM = FOPEN_ERR;
		return;
	}

	fprintf(file, "digraph dump_graph {\n\trankdir=TV; \n");

	TreeTraverseIn(root);
	ERRNUM_CHECK(;);

	fprintf(file, "}\n");
		
	fclose(file);

	system("dot -Tsvg dump/dump.dot -o dump/dump.svg");
}


static int TreeTraverseDelete(TNODE *node)
{
	ERRNUM_CHECK(ERRNUM);

	if (!node)
		return ERRNUM = TREE_NULL_NODE;
	
	if (node->left)
		TreeTraverseDelete(node->left);
	
	if (node->right)
		TreeTraverseDelete(node->right);
	
	free(node);

	return 0;
}

static int TreeTraverseIn(TNODE *node)
{
	ERRNUM_CHECK(ERRNUM);

	if (!node)
		return ERRNUM = TREE_NULL_NODE;
	
	if (node->left)
		TreeTraverseIn(node->left);
	
	TreeDotDump(node);
	VisitPrint(node);

	if (node->right)
		TreeTraverseIn(node->right);
	return 0;
}

void set_node(TNODE *node, tval_t val, TNODE *parent, TNODE *left, TNODE *right)
{
	if (node == NULL)
		return;

	node->data   = val;
	node->parent = parent;
	node->left   = left;
	node->right  = right;
}

int _tree_check(TNODE *node)
{
	CHECK_(node == NULL,  							TREE_NULL_NODE);	
	CHECK_((node->left == node->right) && (node->left != NULL),  		TREE_SAME_CHILD);
	
	return NO_ERR;
}

int TreeCheck(TNODE *node)
{
#ifdef TREE_CHECK_DUMP
	file = fopen("dump/dump.dot", "w");
	assert(file);
#endif

#ifdef VALIDATE_TREE_INTEGRY
	static int flag_visit = 1;
	visit_check(node, flag_visit);
#else
	visit_check(node);
#endif
	ERRNUM_CHECK(ERRNUM);

#ifdef VALIDATE_TREE_INTEGRY
	flag_visit++;
#endif

#ifdef TREE_CHECK_DUMP
	fclose(file);
#endif
	return 0;
}

static void visit_check(TNODE *node, int flagval)
{
	_tree_check(node);
	ERRNUM_CHECK(;);

#ifdef VALIDATE_TREE_INTEGRY
	
	if (node->visit_flag == flagval)
		ERRNUM = TREE_LOOP_FOUND;
	ERRNUM_CHECK(;);
	
	node->visit_flag = flagval;
#endif

#ifdef TREE_CHECK_DUMP
	VisitPrint(node, file);
#endif
	if (node->left)
		visit_check(node->left, flagval);
	
	if (node->right)
		visit_check(node->right, flagval);
}

void VisitPrint(TNODE *node, FILE *fout)
{
	if (!fout)
		return;
	if (!node)
		return;
	
	fprintf(fout, "node : [  %p  ], datatype : [  %d  ], ", node, node->data.data_type);
	if (node->data.data_type == CONST)
		fprintf(fout, "data : [  %lg  ], ", node->data.value.num);
	else 
		fprintf(fout, "data : [  %c  ], ", node->data.value.str);

	fprintf(fout, "\tleft : [  %p  ],\tright : [  %p  ],\tparent : [  %p  ]\n", 
			node->left, node->right, node->parent);
}

void TreeDotDump(TNODE *node)
{
	if (!node || !file)
		return;

	fprintf(file, "node%p [shape=plaintext\n\
			\t\tlabel=<<table border='1' cellborder='1'>\n",node);
	
	switch (node->data.data_type) {
	case CONST:
		fprintf(file, "	\t\t<tr><td colspan=\"2\" bgcolor=\"lightskyblue\" >%lg", node->data.value.num);
		break;
	case OPER:
		fprintf(file, "	\t\t<tr><td colspan=\"2\" bgcolor=\"olivedrab3\" >%c", node->data.value.str);
		break;
	case VAR:
		fprintf(file, "	\t\t<tr><td colspan=\"2\" bgcolor=\"orchid\" >%c", node->data.value.str);
		break;
	default:
		break;
	}

	fprintf(file,   "</td></tr>\n\
			\t\t<tr><td port= \"lchild\">L:%p</td><td port=\"rchild\">R: %p</td></tr>\n\
			\t</table>>];\n", node->left, node->right);
	
	if (node->left)
		fprintf(file, "\t\tnode%p:lchild -> node%p[style=bold, arrowhead=vee];\n",
			       	node, node->left);

	if (node->right)
		fprintf(file, "\t\tnode%p:rchild -> node%p[style=bold, arrowhead=vee];\n", 
				node, node->right);
#ifdef PARENT_PRINT
	if (node->parent)
		fprintf(file, "\t\tnode%p -> node%p[style=bold, arrowhead=vee];\n", 
				node, node->parent);
#endif
}

static int tree_copy(TNODE *src, TNODE **node, TNODE *parent)
{
	ERRNUM_CHECK(ERRNUM);
	
	TreeCtor(node, src->data, parent);
	ERRNUM_CHECK(ERRNUM);
		
	if (src->left)
		tree_copy(src->left,  &((*node)->left), *node);
	
	if (src->right)
		tree_copy(src->right, &((*node)->right), *node);
	
	return ERRNUM;
}

diff_data set_diff_data(int data_type, DATA val)
{
	diff_data ddata = {};

	ddata.value     = val;
	ddata.data_type = data_type;

	return ddata;
}

DATA data_un_d(double num)
{
	DATA tmp  = {};
	tmp.num = num;
	return tmp;
}

DATA data_un_c(char str)
{
	DATA tmp = {};
	tmp.str  = str;
	return tmp;
}
