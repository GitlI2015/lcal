#include "mathconst.h"
#ifndef __COMPUTER_H_
#define __COMPUTER_H_






typedef struct exp
{
    char chr[MAX_SIZE];
    double num[MAX_SIZE];
} EXP;

/*

			mathematic expression consistss of
			digit numbers and mathematic charactors.
			(ignoring brackets)charactors and numbers
			appears one by one

 */



int stkl[BRKT_MAX] = {0};
int stkr[BRKT_MAX] = {0};
int lctl, rctl;
int lbrkt = 0;

/*

			stacks

 */


int Match(int lb_pos);

double SubCompute(char *, int );
double Core_Cmp(EXP *);

double StringToFloat(char *, int *);

double MacroReplace(char *, int *);
int StrMatch(const char *, const char *);
int IsInteger(double );

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
