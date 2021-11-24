#include "differentiator.h"

static int node_print(TNODE *node, FILE *file = stdout);
static int read_database(TNODE **node, TNODE *parent, textBuff *btext, int ip);
static int read_num(TNODE **node, TNODE *parent, textBuff *btext, int ip);
static int read_oper(TNODE **node, TNODE *parent, textBuff *btext, int ip);
static int ident_data_type(textBuff *btext, int ip);
static int const_simplify(TNODE *node, int *change_flag);
static int node_simplify(TNODE *node, int *change_flag);
static int replace_const(TNODE *node, double val);
static int cmp_double(double val1, double val2);
static int replace_nodes(TNODE *dst, TNODE *src);

FILE *diff_log = stdout;

int DiffProcess()
{
	TNODE *tree = NULL;
	textBuff btext = {};

	DiffInit(&tree, &btext);
	printf("\nRaw input : ");
	DiffPrint(tree);
	printf("\n");
	
	DiffTreeSimplify(tree);
	printf("Simplificate input: ");
	DiffPrint(tree);
	printf("\n\n");

	TNODE *answ = DIFF(tree);
	printf("Raw answer: ");
	DiffPrint(answ);
	printf("\n");
	
	DiffTreeSimplify(answ);
	printf("Simplificate answer : ");
	DiffPrint(answ);
	printf("\n");
	TreeDump(answ);

	TreeDtor(answ);
	TreeDtor(tree);
	return 0;
}

TNODE *DIFF(TNODE *node, TNODE *parent) 
{
	ERRNUM_CHECK(NULL);

	switch (TYPE(node)) {
	case CONST:
		return NEW_CONST(0);
		break;
	case VAR:
		return NEW_CONST(1.0);
		break;
	case OPER:
		switch (STR(node)) {
		case OP_ADD:	
			return ADD(DL, DR);
		case OP_MUL:
			return ADD(MUL(DL, CR), MUL(CL, DR));
		case OP_SUB:
			return SUB(DL, DR); 
		case OP_PWR:
			return MUL(CR, PWR(CL, SUB(CR, NEW_CONST(1))));
		case OP_DIV:
			return DIV(SUB(MUL(DL, CR), MUL(DR, CL)), PWR(CR, NEW_CONST(2)));
		default:
			break;
		}
		break;
	case UOPER:
		switch (STR(node)) {
		case UOP_SIN:
			return 0;
		}
	default:
		ERRNUM = DIFF_UNKNOWN_TYPE;
		return NULL;
		break;
	}
	return NULL;
}

int DiffPrint(TNODE *node, FILE *fout)
{
	ERRNUM_CHECK(ERRNUM);
	TREE_CHECK(node, ERRNUM);

	if (!node->left && !node->right) {
		node_print(node, fout);
		return 0;
	}

	fprintf(fout, "(");
	
	if(node->left)
		DiffPrint(node->left, fout);
	
	node_print(node, fout);
	
	if(node->right)
		DiffPrint(node->right, fout);
	
	fprintf(fout, ")");
	return 0;
}

int DiffTreeSimplify(TNODE *root)
{
	TREE_CHECK(root, ERRNUM);

	int is_changed = 0;
	do {
		is_changed = 0;

		const_simplify(root, &is_changed);
		node_simplify(root,  &is_changed);
	} while (is_changed);
	
	return 0;
}

static int const_simplify(TNODE *node, int *change_flag)
{
	ERRNUM_CHECK(UNKNOWN_TYPE);
	
	if (!node)
		return UNKNOWN_TYPE;

	if (TYPE(node) == OPER) {

		int lchild_t = const_simplify(node->left,  change_flag);
		int rchild_t = const_simplify(node->right, change_flag);

		if (lchild_t == CONST && rchild_t == CONST) {
			*change_flag = 1;

			switch (STR(node)) {
			case OP_ADD:
				REPLACE_CONST(node, +);
				break;
			case OP_MUL:
				REPLACE_CONST(node, *);
				break;
			case OP_DIV:
				REPLACE_CONST(node, /);
				break;
			case OP_SUB:
				REPLACE_CONST(node, -);
				break;
			case OP_PWR:
				replace_const(node, pow(NUM(node->left), NUM(node->right)));
				break;
			default:
				ERRNUM = DIFF_UNKNOWN_TYPE;
				return UNKNOWN_TYPE;
			}

			return CONST;
		}
	}
	
	return TYPE(node);
}

static int replace_const(TNODE *node, double val)
{
	CHECK_(node == NULL, TREE_NULL_NODE);
	
	TreeDtor(node->left);
	TreeDtor(node->right);
	
	node->left  = NULL;
	node->right = NULL;

	NUM(node)  = val;
	TYPE(node) = CONST;

	return 0;
}

#define REPLACE_LR_CONST(node, val)						\
	{									\
		TYPE(node) = CONST;						\
		NUM(node)  = val;						\
		TreeDtor(node->left);						\
		TreeDtor(node->right);						\
		node->left  = NULL;						\
		node->right = NULL; 						\
		*change_flag = 1;						\
	}

#define REPLACE_PARENT(node, node_del, node_rep)				\
	{									\
		TreeDtor(node_del);						\
		replace_nodes(node, node_rep);					\
		*change_flag = 1;						\
	}

static int node_simplify(TNODE *node, int *change_flag)
{
	ERRNUM_CHECK(UNKNOWN_TYPE);
	
	if (!node->left || !node->right || TYPE(node) != OPER)
		return 0;

	switch (STR(node)) {
	case OP_ADD:
		if (IS_NUM(node->left, 0)) {
			REPLACE_PARENT(node, node->left, node->right);
		} else if (IS_NUM(node->right, 0)) {
			REPLACE_PARENT(node, node->left, node->left);
		}
		break;
	case OP_MUL:
		if (IS_NUM(node->left, 0) || IS_NUM(node->right, 0)) {
			REPLACE_LR_CONST(node, 0);
		} else if (IS_NUM(node->left, 1)) {
			REPLACE_PARENT(node, node->left, node->right);
		} else if (IS_NUM(node->right, 1)) {
			REPLACE_PARENT(node, node->right, node->left);
		}
		break;
	case OP_DIV:
		if (IS_NUM(node->left, 0)) {
			REPLACE_LR_CONST(node, 0);
		} else if (IS_NUM(node->right, 1)) {
			REPLACE_PARENT(node, node->right, node->left);
		}
		break;
	case OP_SUB:
		if (IS_NUM(node->right, 0))
			REPLACE_PARENT(node, node->right, node->left);
		break;
	case OP_PWR:
		if (IS_NUM(node->left, 0)){ 
			REPLACE_LR_CONST(node, 0);
		} else if (IS_NUM(node->left, 1)) {
			REPLACE_LR_CONST(node, 1);
		} else if (IS_NUM(node->right, 0)) {
			REPLACE_LR_CONST(node, 1);
		} else if (IS_NUM(node->right, 1)) {
			REPLACE_PARENT(node, node->right, node->left);	
		}
		break;
	}
	if (node->left  && TYPE(node->left)  == OPER)
		node_simplify(node->left, change_flag);
	if (node->right && TYPE(node->right) == OPER)
		node_simplify(node->right, change_flag);
	return 0;
}

#undef REPLACE_PARENT
#undef REPLACE_LR_CONST

static int replace_nodes(TNODE *dst, TNODE *src)
{
	CHECK_(!dst || !src, TREE_NULL_NODE);

	set_node(dst, src->data, dst->parent, src->left, src->right);
	set_node(src, src->data);
	TreeDtor(src);

	return 0;
}
int node_print(TNODE *node, FILE *file)
{
	CHECK_(node == NULL, TREE_NULL_NODE);
	
	switch (node->data.data_type) {
	case CONST:
		fprintf(file, "%lg", NUM(node));
		break;
	case OPER:
		fprintf(file, "%c",  STR(node));
		break;
	case VAR:
		fprintf(file, "%c",  STR(node));
		break;
	default:
		return ERRNUM = DIFF_UNKNOWN_TYPE;
	}
	return 0;
}

TNODE* new_node(int type, DATA val, TNODE *left, TNODE *right, TNODE *parent)
{
	TNODE* node = NULL;
	TreeCtor(&node);

	ERRNUM_CHECK(NULL);

	set_node(node, set_diff_data(type, val), parent, left, right);
	
	if (node->left)
		node->left->parent = node;
	if (node->right)
		node->right->parent = node;


	return node;
}

TNODE *copy_nodes(TNODE *src, TNODE *parent)
{
	TNODE *node = NULL;

	TreeCopy(src, &node, parent);
	ERRNUM_CHECK(NULL);

	return node;
}

int DiffInit(TNODE **tree, textBuff *btext)
{
	CHECK_(!tree,		TREE_NULL_NODE);
	CHECK_(!btext,		AKINATOR_NULL_ARG);
	CHECK_(btext->buff,	AKINATOR_BAD_INIT_STR);

	const char *namein = "diff_in.txt";
	btext->file_in = fopen(namein, "r");
	CHECK_(btext->file_in == NULL, FOPEN_ERR);
	read_from_file(btext, namein);
	
	
	fprintf(diff_log, "%s\n", btext->buff);

	read_database(tree, NULL, btext, 0);
	ERRNUM_CHECK(ERRNUM);
	
	fclose(btext->file_in);
	free(btext->buff);
	return 0;
}

//ZHOPA POLNAYA
static int read_database(TNODE **node, TNODE *parent, textBuff *btext, int ip)
{
	ERRNUM_CHECK(ip);	
	
	printf("New node created\n");
	TreeCtor(node, set_diff_data(-1));
	int left = 0;
	for ( ; ip < btext->buffsize; ip++) {
		printf("[%d]  :  \"%c\"\n", ip, btext->buff[ip]);

		switch (ident_data_type(btext, ip)) {
		case CONST:
			ip = read_num(node, parent, btext, ip);
			if (ip < 0)
				return ERRNUM;
			break;
		case VAR:
			STR(*node) = btext->buff[ip];
			printf("[%d] added variable \'%c\'\n", ip, STR(*node));
			TYPE(*node) = VAR;
			(*node)->parent = parent;
			break;
		case OPER:
			ip = read_oper(node, parent, btext, ip);
			if (ip < 0)
				return ERRNUM;

			break;
		default:
			if (btext->buff[ip] == '(') {
				if (!left && btext->buff[ip + 1] == '(') {
					printf("[%d]  :  left\n", ip);
					ip = read_database(&((*node)->left), *node,  btext, ip + 1);
					printf("[%d]\n", ip);
					left = 1;
				} else if (left == 1){
					printf("[%d]  :  right\n", ip);
//					left = -1;
					ip = read_database(&((*node)->right), *node, btext, ip);
				}
			} else if (btext->buff[ip] == ')') {
				if (ip == 0) {
					ERRNUM = DIFF_SYNTAX_ERR;
					return ip;
				}
				printf("[%d]  :  returned\n", ip);
				VisitPrint(*node, diff_log);
				
				return (ip);

			}
			break;
		}
	}
	
	return ERRNUM = DIFF_SYNTAX_ERR;	
}

static int ident_data_type(textBuff *btext, int ip)
{	
	if (isalpha(btext->buff[ip]) && islower(btext->buff[ip])) {// TODO more if for sin/cos and s.o.
		return VAR;
	} else if (isdigit(btext->buff[ip])) {
		return CONST;
	} else if ((btext->buff[ip] == '(') || (btext->buff[ip] == ')')) {
		return btext->buff[ip];
	} else {
		return OPER;
	}
}

static int read_num(TNODE **node, TNODE *parent, textBuff *btext, int ip)
{
	char *str = NULL;
	double val = strtod((btext->buff + ip), &str);
	
	if (str == NULL && str[0] != ')') {
		ERRNUM = DIFF_SYNTAX_ERR;
		return -1;
	}
	
	printf("[%d] Numeric value %f added\n", ip, val);

	NUM((*node)) = val;
	TYPE(*node) = CONST;
	(*node)->parent = parent;

	for ( ; ip < btext->buffsize && btext->buff[ip] != ')'; ip++)
		;

	return ip - 1;
}

static int read_oper(TNODE **node, TNODE *parent, textBuff *btext, int ip)
{//TODO sin cos log and s.o.  TODO ERRS?
	STR(*node) = btext->buff[ip];

	if (isupper(btext->buff[ip]))
		TYPE(*node) = UOPER;
	else 
		TYPE(*node) = OPER;
	(*node)->parent = parent;

	printf("[%d] operator \'%c\' added\n", ip, STR(*node));
	
	return ip;
}

static int cmp_double(double val1, double val2)
{
	if (fabs(val1 - val2) <= ACCURACY)
		return 0;
	else
		return (val1 - val2);
}
