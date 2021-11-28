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
static int print_tex_node(TNODE *node);
static int op_priority(int op);

FILE *diff_log = stdout;

int DiffProcess()
{
	FILE *file = fopen("dump/texdump.tex", "w");
	CHECK_(file == NULL, FOPEN_ERR);

	fprintf(file, "\\documentclass{article}\n"
		"\\usepackage[utf8]{inputenc}\n"
		"\\title{Differentiatin log}\n"
		"\\author{enikeev.en}\n"
		"\\date{September 2021}\n"
		"\\usepackage[utf8]{inputenc}\n"
		"\\usepackage[T2A]{fontenc}\n"
		"\\usepackage[russian]{babel}\n"
		"\\usepackage{amsthm}\n"
		"\\usepackage{amsmath}\n"
		"\\usepackage{amssymb}\n"
		"\\usepackage{tikz}\n"
		"\\usepackage{textcomp}\n"
		"\\usepackage{marvosym}\n"
		"\\usepackage{ esint }\n"
		"\\usepackage{amsfonts}\n"
		"\\setlength{\\topmargin}{-0.5in}\n"
		"\\setlength{\\textheight}{9.1in}\n"
		"\\setlength{\\oddsidemargin}{-0.4in}\n"
		"\\setlength{\\evensidemargin}{-0.4in}\n"
		"\\setlength{\\textwidth}{7in}\n"
		"\\setlength{\\parindent}{0ex}\n"
		"\\setlength{\\parskip}{1ex}\n"
		"\\begin{document}");
		
	TNODE *tree = NULL;
	TNODE *answ = NULL;

	textBuff btext = {};
	do {
		DiffInit(&tree, &btext);
		fprintf(file, "\\Large \\center\n\tRaw input.\n\\[");
		DiffPrint(tree, file);
		fprintf(file, "\\]\n\n");
		CHECK_BREAK(ERRNUM);

		DiffTreeSimplify(tree);
		fprintf(file, "\\Large \\center\n\tSimplificate input.\n\\[");	
		DiffPrint(tree, file);
		fprintf(file, "\\]\n\n");
		CHECK_BREAK(ERRNUM);

		answ = DIFF(tree);
		fprintf(file, "\\Large \\center\n\tRaw answer.\n\\[");
		DiffPrint(answ, file);
		fprintf(file, "\\]\n\n");
		CHECK_BREAK(ERRNUM);
		
		DiffTreeSimplify(answ);
		fprintf(file, "\\Large \\center\n\tSimplificate answer.\n\\[");
		DiffPrint(answ, file);
		fprintf(file, "\\]\n\\end {document}");
		CHECK_BREAK(ERRNUM);	
		
		TreeDump(tree);
	} while(0);

	if (file != stdout)
		fclose(file);
	
	TreeDtor(answ);
	TreeDtor(tree);
	return ERRNUM;
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
			return MUL(DL, MUL(CR, PWR(CL, SUB(CR, NEW_CONST(1)))));
		case OP_DIV:
			return DIV(SUB(MUL(DL, CR), MUL(DR, CL)), PWR(CR, NEW_CONST(2)));
		default:
			break;
		}
		break;
	case UOPER:
		switch (STR(node)) {
		case UOP_SIN:
			return MUL(DL, COS(CL));
		case UOP_COS:
			return MUL(NEW_CONST(-1), MUL(DL, SIN(CL)));
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
	
	if (TYPE(node) == UOPER) {
		node_print(node, fout);
		fprintf(fout, "(");
	}

	int visit_left  = 1;
	int visit_right = 1;
	
	if (TYPE(node) == OPER) {
		switch (STR(node)) {
		case OP_DIV:
			fprintf(fout, "\\frac{");
			break;
		case OP_MUL:
			if (node->left && TYPE(node->left) == CONST && NUM(node->left) < 0) 
				fprintf(fout, "(");

			if (TYPE(node->left) == OPER && op_priority(STR(node->left)) == PR_ADSB)
				fprintf(fout, "(");
						break;
		case OP_PWR:
			if (TYPE(node->left) != CONST && TYPE(node->left) != VAR)
				fprintf(fout, "(");
			break;
		case OP_ADD:
		case OP_SUB:
			if (TYPE(node->left) == CONST && NUM(node->left) == 0)
				visit_left = 0;
			break;
		default:
			fprintf(fout, "(");
			break;
		}
	}

///////////////////////////////////////////////
	if(visit_left && node->left)
		DiffPrint(node->left, fout);
///////////////////////////////////////////////
	
	if (TYPE(node) == OPER) {
		switch (STR(node)) {
		case OP_DIV:
			fprintf(fout, "}{");
			break;
		case OP_PWR:
			if (TYPE(node->left) != CONST && TYPE(node->left) != VAR)
				fprintf(fout, ")");
			fprintf(fout, "^{");
			break;
		case OP_MUL:
			if (node->left && TYPE(node->left) == CONST && NUM(node->left) < 0) 
				fprintf(fout, ")");
			
			if (TYPE(node->left) == OPER && op_priority(STR(node->left)) == PR_ADSB)
				fprintf(fout, ")");

			fprintf(fout, "\\cdot ");
			
			if (node->right && TYPE(node->right) == CONST && NUM(node->right) < 0) 
				fprintf(fout, "(");
			
			if (TYPE(node->right) == OPER && op_priority(STR(node->right)) == PR_ADSB)
				fprintf(fout, "(");
			break;
		case OP_ADD:
		case OP_SUB:
			if (TYPE(node->right) == CONST && NUM(node->right) == 0)
				visit_right = 0;
			
			if (STR(node) == OP_ADD) {
				if (visit_left == 0)
					break;	
			}

			if (node->right && TYPE(node->right) == OPER && STR(node->right) == OP_SUB 
					&& TYPE(node->right->left) == CONST && NUM(node->right->left) == 0)
				break;

			if (node->right && TYPE(node->right) == CONST && NUM(node->right) < 0) {
				if (STR(node) == OP_ADD)
					break;
				else 
					fprintf(fout, "(");
			}

			node_print(node, fout);
			break;

		default:
			node_print(node, fout);
			break;
		}
	}

///////////////////////////////////////////////
	if(visit_right && node->right)
		DiffPrint(node->right, fout);
///////////////////////////////////////////////
	
	if (TYPE(node) == OPER) {
		switch (STR(node)) {
		case OP_DIV:
			fprintf(fout, "}");
			break;
		case OP_PWR:
			fprintf(fout, "}");
			break;
		case OP_MUL:
			if (node->right && TYPE(node->right) == CONST && NUM(node->right) < 0) 
				fprintf(fout, ")");

			if (TYPE(node->right) == OPER && op_priority(STR(node->right)) == PR_ADSB)
				fprintf(fout, ")");
			break;
		case OP_ADD:
		case OP_SUB:
			if (node->right && TYPE(node->right) == CONST && NUM(node->right) < 0) {
				fprintf(fout, ")");
			}
			break;
		default:
			fprintf(fout, ")");
			break;
		}
	}

	if (TYPE(node) == UOPER) {
		fprintf(fout, ")");
	}

	return 0;
}

static int op_priority(int op)
{
	switch (op) {
	case OP_ADD:
	case OP_SUB:
		return PR_ADSB;
	case OP_MUL:
	case OP_DIV:
		return PR_MUDI;
	case PR_PWR:
		return PR_PWR;
	default:
		return -1;
	}
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
		if (!node->left)
			return TYPE(node);

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
	ERRNUM_CHECK(ERRNUM);
	CHECK_(node == NULL, TREE_NULL_NODE);
	
	switch (node->data.data_type) {
	case CONST:
		fprintf(file, "%lg", NUM(node));
		break;
	case OPER:
		fprintf(file, " %c ",  STR(node));
		break;
	case VAR:
		fprintf(file, "%c",  STR(node));
		break;
	case UOPER:
		switch(STR(node)) {
		case UOP_SIN:
			fprintf(file, "sin");
			break;
		case UOP_COS:
			fprintf(file, "cos");
			break;
		default:
			return ERRNUM = DIFF_UNKNOWN_TYPE;
		}
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
		VisitPrint(*node, diff_log);

		switch (ident_data_type(btext, ip)) {
		case CONST:
			ip = read_num(node, parent, btext, ip);
			if (ip < 0)
				return ERRNUM;
			printf("[%d]  :  returned\n", ip);
			return ip;
			break;
		case VAR:
			STR(*node) = btext->buff[ip];
			printf("[%d] added variable \'%c\'\n", ip, STR(*node));
			TYPE(*node) = VAR;
			(*node)->parent = parent;
			printf("[%d]  :  returned\n", ip);
			VisitPrint(*node, diff_log);

			return ip;
			break;
		case OPER:
			ip = read_oper(node, parent, btext, ip);
			if (ip < 0)
				return ERRNUM;
			if (TYPE(*node) == OPER)
				ip = read_database(&((*node)->right), *node,  btext, ip + 1);
			
			break;
		default:
			if (btext->buff[ip] == '(') {
				ip = read_database(&((*node)->left), *node,  btext, ip + 1);
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
	$	
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

	for ( ; ip < btext->buffsize && (isdigit(btext->buff[ip]) || btext->buff[ip] == '.'); ip++)
		;

	return ip - 1;
}

static int read_oper(TNODE **node, TNODE *parent, textBuff *btext, int ip)
{
	STR(*node) = btext->buff[ip];

	if (isupper(btext->buff[ip])) {
		TYPE(*node) = UOPER;
	}
	else {
		TYPE(*node) = OPER;
	}
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
