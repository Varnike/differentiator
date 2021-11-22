#include "differentiator.h"
#include "tree.h"

int main()
{
	TNODE *root = NULL;
	
	//TreeCtor(&root, set_diff_data(OPER, data_un('^')));
	//TreeCtor(&(root->right), set_diff_data(CONST, data_un(2.0)), root);
	//TreeCtor(&(root->left), set_diff_data(OPER, data_un('+')), root);
	//TreeCtor(&(root->left->left), set_diff_data(VAR, data_un('x')), root->left);
	//TreeCtor(&(root->left->right), set_diff_data(CONST, data_un(1.0)), root->left);
	/*	
	TNODE *new_tree = NULL;
	TreeCopy(root, &new_tree, new_tree);
	
	textBuff btext = {};
	TNODE *node1 = NULL;
	DiffInit(&node1, &btext);
	TreeDump(node1);
	DiffPrint(node1);
	printf("\n");
	TreeDtor(node1);
	*/
	DiffProcess();

	return 0;
}
