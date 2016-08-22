#include<stdio.h>
#include<ctype.h>
#include<stdlib.h>
#include<string.h>
#include"mathconst.h"


int GetExpression(char *);
void Compute(char *, int );
void ModeIn(char * );


double * Answers;
int Answer_Counter = 1;
int err_sgn = 0;
int exit_flag = 0;

/*
			declaration
 */

int main()
{
    char args[MAX_SIZE];
	memset(args,sizeof(char),MAX_SIZE);
    puts("mini-calculator by li, 2015");
    puts("enter \"help\" to get manual                               ");
    Answers = (double *)malloc(1024 * 10 * sizeof(double));
    while (!exit_flag)
    {
        switch (GetExpression(args))
        {
        case -2:
			free(Answers);
			exit_flag = 1;
        case -3:
            ModeIn(args + 1);
            continue;
        default:
            Compute(args, strlen(args));
            //puts(args);
        }
    }
    return 0;
}


// GetExpression expression and check brackets
int GetExpression(char *args)
{
    int i = 0;
    printf(" Input[%d]:", Answer_Counter);
    while ((args[i] = getchar()) != '\n' && i != MAX_SIZE)
        i++;
    args[i] = '\0';
    if (i == 0) return -2;
    if (args[0] == '!') return -3;
    return 0;
}


