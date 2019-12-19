#define NSYMS 200  /* maximum number of symbols */

struct{
	char args;
	double value;
	char* name;
	int is_variable;
}symtab[NSYMS];

struct symtab *symlook(char* s);