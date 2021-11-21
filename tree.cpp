#include "tree.h"

static void set_node(TNODE *node, tval_t val, TNODE *left = NULL, TNODE *right = NULL);
static void visit_check(TNODE *node, int flagval = 0);

static int TreeTraversePre(TNODE *root);
static int TreeTraverseDelete(TNODE *root);

int _tree_check(TNODE *node);

FILE *file = NULL;

int TreeCtor(TNODE **root, tval_t val)
{
	CHECK_(*root != NULL, TREE_BAD_CTOR_ROOT);

	//assert(root == NULL);

	*root = (TNODE*)calloc(1,sizeof(TNODE));

	if (*root == NULL)
		return ERRNUM = CALLOC_ERR;

	
	set_node(*root, val);
	
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

int TreeDtor(TNODE *node) 
{
	TREE_CHECK(node, ERRNUM);
	
	TreeTraverseDelete(node);
	ERRNUM_CHECK(ERRNUM);	

	return 0;
}
//TODO ERRS
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

static int TreeTraversePre(TNODE *node)
{
	ERRNUM_CHECK(ERRNUM);

	if (!node)
		return ERRNUM = TREE_NULL_NODE;
	
	if (node->left)
		TreeTraversePre(node->left);
	
	TreeDotDump(node);
		
	if (node->right)
		TreeTraversePre(node->right);
	return 0;
}

void TreeDump(TNODE *root)
{
	TREE_CHECK(root, ERRNUM);

	file = fopen("dump/dump.dot", "w");
	assert(file);

	if (file == NULL) {
		ERRNUM = FOPEN_ERR;
		return;
	}

	fprintf(file, "digraph dump_graph {\n\trankdir=TV; \n");

	TreeTraversePre(root);
	ERRNUM_CHECK(;);

	fprintf(file, "}\n");
		
	fclose(file);

	system("dot -Tsvg dump/dump.dot -o dump/dump.svg");
}

static void set_node(TNODE *node, tval_t val, TNODE *left, TNODE *right)
{
	if (node == NULL)
		return;

	node->data = val;
	node->left = left;
	node->right = right;
}

int _tree_check(TNODE *node)
{
	CHECK_(node == NULL,  						TREE_NULL_NODE);
	CHECK_(node->data.strptr == NULL, 				TREE_NULL_DATA);
	CHECK_((node->left == node->right) && (node->left != NULL),  	TREE_SAME_CHILD);
	
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

	fprintf(fout, "node : [  %p  ], data : [%s], len : [  %d  ], left : [  %p  ], right : [  %p  ]\n", 
		node, node->data.strptr, node->data.len, node->left, node->right);
}

void TreeDotDump(TNODE *node)
{
	if (!node || !file)
		return;

	fprintf(file, "node%p [shape=plaintext\n\
			\t\tlabel=<<table border='1' cellborder='1'>\n\
			\t\t<tr><td colspan=\"2\" bgcolor=\"lightskyblue\" >%s</td></tr>\n\
			\t\t<tr><td port= \"lchild\">L:%p</td><td port=\"rchild\">R: %p</td></tr>\n\
			\t</table>>];\n", node, node->data.strptr, node->left, node->right);
	
	if (node->left)
		fprintf(file, "\tnode%p:lchild -> node%p[style=bold, arrowhead=vee label = \"Yes\"];\n",
			       	node, node->left);

	if (node->right)
		fprintf(file, "\tnode%p:rchild -> node%p[style=bold, arrowhead=vee label = \"No\"];\n", 
				node, node->right);
}
