#include "mathconst.h"
#ifndef __COMPUTER_H_
#define __COMPUTER_H_





struct val_t
{
    void *value;
    enum {matrix, normal, function} val_type;
};

struct var_pair
{
    char var_name[20];
	struct val_t val;
};

struct var_table
{
    void *this;
    struct var_pair* contents;
    int size;
    int max_size;
    void (*resize)(void *);
};


int find_var(struct var_table *, char *);
struct var_pair *get_var(struct var_table *, int );



void Mode(char *);
void Reset(void);

void GetHelp(void);


/*
			declaration
 */



long long int Factorial(int , int );

/*
		math function declaration
 */


//Compute mathematic expression by backtracking



#endif
