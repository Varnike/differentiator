#include "differentiator.h"

static int node_print(TNODE *node, FILE *file = stdout);
static int read_database(TNODE **node, TNODE *parent, textBuff *btext, int ip);
static int read_num(TNODE **node, TNODE *parent, textBuff *btext, int ip);
static int read_oper(TNODE **node, TNODE *parent, textBuff *btext, int ip);
static int ident_data_type(textBuff *btext, int ip);

static TNODE* new_node(int type, DATA val, TNODE *right, TNODE *left, TNODE *parent);

FILE *diff_log = stdout;

int DiffProcess()
{
	TNODE *tree = NULL;
	textBuff btext = {};
$
	DiffInit(&tree, &btext);
	TreeDump(tree);

	DiffPrint(tree);
	printf("\n");
	TreeDtor(tree);
	return 0;
}
/*
TNODE *DIFF(TNODE *node) 
{
	switch (src->data.data_type) {
	case CONST:
		return NEW_NODE();
		break;
	case VAR:
		break;
	case OPER:
		break;
	default:
		return ERRNUM = DIFF_UNKNOWN_TYPE;
		break;
	}
}
*/
int DiffPrint(TNODE *node, FILE *fout)
{
	ERRNUM_CHECK(ERRNUM);

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
/*
static TNODE* new_node(int type, DATA val, TNODE *right, TNODE *left, TNODE *parent)
{
	TNODE* node = NULL;
	TreeCtor(&node, set_diff_data(type, val), parent);

	node->set
}
*/
int DiffInit(TNODE **tree, textBuff *btext)
{
	CHECK_(!tree,		TREE_NULL_NODE);
	CHECK_(!btext,		AKINATOR_NULL_ARG);
	CHECK_(btext->buff,	AKINATOR_BAD_INIT_STR);

	const char *namein = "diff_in.txt";
	btext->file_in = fopen(namein, "r");
	CHECK_(btext->file_in == NULL, FOPEN_ERR);
	read_from_file(btext, namein);
	
	
$	fprintf(diff_log, "%s\n", btext->buff);

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
			printf("\t[ip = %d] added variable \'%c\'\n", ip, STR(*node));
			(*node)->data.data_type = VAR;
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
	if (isalpha(btext->buff[ip])) {// TODO more if for sin/cos and s.o.
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
	
	printf("\t[ip = %d] Numeric value %f added\n", ip, val);

	NUM((*node)) = val;
	(*node)->data.data_type = CONST;
	(*node)->parent = parent;

	for ( ; ip < btext->buffsize && btext->buff[ip] != ')'; ip++)
		;

	return ip - 1;
}

static int read_oper(TNODE **node, TNODE *parent, textBuff *btext, int ip)
{//TODO sin cos log and s.o.  TODO ERRS?
	
	STR(*node) = btext->buff[ip];
	(*node)->data.data_type = OPER;
	(*node)->parent = parent;

	printf("\t[ip = %d] Operator \'%c\' added\n", ip, STR(*node));
	
	return ip;
}
