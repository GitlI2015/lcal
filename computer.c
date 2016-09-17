#include<stdio.h>
#include<ctype.h>
#include<stdlib.h>
#include<string.h>

#if defined __unix
#include<readline/readline.h>
#include<readline/history.h>
#endif

#include"mathconst.h"
#include"computer.h"


int GetExpression(char *);
void Compute(char *, int );


double *Answers;
struct var_table table;
int Answer_Counter = 1;
int err_sgn = 0;
int exit_flag = 0;
char *complete_list[MAX_SIZE] = {"sin", "cos", "tan", "asin", "acos", "atan", \
                                 "sinh", "cosh", "tanh", "asinh", "acosh", "atanh", "log",\
                                 "exit", "clear"\
                                };

/*
			declaration
 */
#if defined __unix
char *func_generator(const char *, int);
char **lcal_completion(const char *, int , int);
int list_size = BASE_SIZE;
char *dupstr(const char *);

#endif
int main()
{
    char args[MAX_SIZE];
    char *input;
    memset(args, sizeof(char), MAX_SIZE);

#if defined __unix
    rl_attempted_completion_function = lcal_completion;
#endif

    puts("mini-calculator by li, 2016");
    puts("enter \"help\" to get manual");
    Answers = (double *)malloc(1024 * 10 * sizeof(double));
    table.max_size = 10;
    table.size = 0;
    table.contents = (struct var_pair *)malloc(10 * sizeof(struct var_pair));
    table.this = &table.this;

    while (!exit_flag) {

#if defined __unix
        snprintf(args, MAX_SIZE, " Input[%d]:", Answer_Counter);
        input = readline(args);

        if(!input)
            break;
		if(strlen(input) == 0)
			continue;
        Compute(input, strlen(input));
        add_history(input);
#else
        GetExpression(args);
        Compute(args, strlen(args));
#endif

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

#if defined __unix
char **lcal_completion(const char *text, int start, int  end)
{
    char **matches;
    matches = (char **)NULL;

    rl_completion_append_character = '\0';

    if(start == 0)
        matches = rl_completion_matches(text, func_generator);

    return matches;
}
char *func_generator(const char *text, int state)
{
    static int list_index, len;
    const char *name;

    if(!state) {
        list_index = 0;
        len = strlen(text);
    }

    while(name = complete_list[list_index]) {
        if(list_index == list_size)
            break;

        list_index++;

        if(strncmp(name, text, len) == 0)
            return(dupstr(name));
    }

    return (char *)NULL;
}

char *dupstr(const char *s)
{
    char *r = (char *)malloc((strlen(s) + 1));
    strcpy(r, s);
    return r;
}
#endif
