#include<stdio.h>
#include<ctype.h>
#include<stdlib.h>
#include<string.h>
#include"mathconst.h"
#include"computer.h"


int GetExpression(char *);
void Compute(char *, int );


double *Answers;
struct var_table table;
int Answer_Counter = 1;
int err_sgn = 0;
int exit_flag = 0;

/*
			declaration
 */

int main()
{
    char args[MAX_SIZE];
    memset(args, sizeof(char), MAX_SIZE);
    puts("mini-calculator by li, 2016");
    puts("enter \"help\" to get manual");
    Answers = (double *)malloc(1024 * 10 * sizeof(double));
    table.max_size = 10;
    table.size = 0; table.contents = (struct var_pair*)malloc(10 * sizeof(struct var_pair)); table.this=&table.this; 
    while (!exit_flag)
    {
        switch (GetExpression(args))
        {
        case -2:
            free(Answers);
            exit_flag = 1;
        case -3:
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


