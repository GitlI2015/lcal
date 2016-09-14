#include"computer.h"
#include<stdio.h>
#include<string.h>
#include<stdlib.h>
#include<ctype.h>
#include<math.h>
#include<complex.h>


extern double *Answers;
extern struct var_table table;
extern int Answer_Counter;
extern int err_sgn;
extern int exit_flag;

typedef struct exp {
    char chr[MAX_SIZE];
    double num[MAX_SIZE];
} EXP;

/*

			mathematic expression consistss of
			digit numbers and mathematic charactors.
			(ignoring brackets)charactors and numbers
			appears one by one

 */

int Match(int lb_pos);

double SubCompute(char *, int );
double Core_Cmp(EXP *);

double StringToFloat(char *, int *);

double MacroReplace(char *, int *);
int StrMatch(const char *, const char *);
int IsInteger(double );

void pFloat(double ); //print float with proper digits

int stkl[BRKT_MAX] = {0};
int stkr[BRKT_MAX] = {0};
int lctl, rctl;
int lbrkt = 0;

/*

			stacks

 */


#if defined __unix
extern char *complete_list[];
extern int list_size;
#endif

void Compute(char *args, int max)
{
    Reset();
    int i;

    for (lctl = 0, i = 0, rctl = 0; (unsigned)i < strlen(args); i++) {
        if (args[i] == '(')
            stkl[lctl++] = i;
        else if (args[i] == ')')
            stkr[rctl++] = i;

        if (rctl > lctl)
            break;
    }

    i = 0;

    if (lctl != rctl) {
        printf("Brackets don't match\n");
        return;
    }

    Answers[Answer_Counter - 1] = SubCompute(args, max);

    if (!err_sgn) {
        if(IsInteger(Answers[Answer_Counter - 1]))
            printf("Output[%d]:%lld\n", Answer_Counter, (long long)Answers[Answer_Counter - 1]);
        else
		{
            printf("Output[%d]:", Answer_Counter);
			pFloat(Answers[Answer_Counter - 1]);
			putchar('\n');
		}

        Answer_Counter++;
    }

    return;
}




double SubCompute(char *args, int max)

{
    int i, temp;
    int chr_ctl = 0, num_ctl = 0;
    double result = 0;
    EXP *mexp = (EXP *)malloc(sizeof(EXP));
    i = 0;

    if(strlen(args) == (unsigned)max) {
        if (StrMatch(args, "help")) {
            GetHelp();
            err_sgn = 1;
            return 0;
        } else if(StrMatch(args, "exit")) {
            err_sgn = 1;
            exit_flag = 1;
            return 0;
        } else if(StrMatch(args, "clear")) {
			printf("Variable table has been cleared.\n");
			err_sgn = 1;
			table.size = 0;
			return 0;
        }
    }

    if(args[i] == '-') {
        mexp->num[num_ctl++] = -1;
        mexp->chr[chr_ctl++] = '*';
        i++;
    }

    while (i < max) {
        if (isdigit(args[i]))
            mexp->num[num_ctl++] = StringToFloat(args, &i);
        else if (StrMatch(args + i, "mod")) {
            mexp->chr[chr_ctl++] = '%';
            i += 2;
        } else if (isalpha(args[i]))
            mexp->num[num_ctl++] = MacroReplace(args, &i);
        else {
            switch (args[i]) {
            case ')':
            case ']':
            case ' ':
                break;

            case '(':
                if(num_ctl - chr_ctl == 1)
                    mexp->chr[chr_ctl++] = '*';

                temp = lbrkt;
                mexp->num[num_ctl++] = SubCompute(args + i + 1, Match(lbrkt++));
                i += Match(temp) + 1;
                break;

            case '+':
            case '-':
            case '*':
            case '/':
                mexp->chr[chr_ctl++] = args[i];
                break;

            case '!':
                if (IsInteger(mexp->num[num_ctl - 1])) {
                    if(mexp->num[num_ctl - 1] < 0) {
                        printf("The parameter of Factorial function should be a natural number.\n");
                        err_sgn = 1;
                        return 0;
                    }

                    if (args[i + 1] == '!') {
                        i++;
                        mexp->num[num_ctl - 1] = (double)Factorial((int)mexp->num[num_ctl - 1], 0);
                    } else
                        mexp->num[num_ctl - 1] = (double)Factorial((int)mexp->num[num_ctl - 1], 1);

                    break;
                } else {
                    printf("The parameter of Factorial function should be an integer.\n");
                    err_sgn = 1;
                    return 0;
                }

            case '^': {
                mexp->chr[chr_ctl++] = '^';

                if(args[i + 1] == '-') {
                    i++;
                    mexp->num[num_ctl++] = StringToFloat(args, &i);
                }

                break;
            }

            case '%': {
                if(isdigit(args[i + 1]))
                    temp = StringToFloat(args + 1, &i);
                else
                    temp = Answer_Counter;

                i++;

                if (IsInteger((double)temp)) {
                    if (temp >= Answer_Counter) {
                        printf("usage:\"%%num\"  num:the number should be less than current order number\n");
                        err_sgn = 1;
                        return 0;
                    } else {
                        mexp->num[num_ctl++] = Answers[temp - 1];
                        break;
                    }
                } else {
                    printf("usage:%%number  number:the number should be an integer");
                    err_sgn = 1;
                    break;
                }
            }

            default:
                printf("I can't understand \"%c\"\n", args[i]);
                err_sgn = 1;
            }
        }

        i++;

        if (err_sgn)
            break;
    }

    mexp->chr[chr_ctl] = '\0';

    if (chr_ctl + 1 == num_ctl)
        result = Core_Cmp(mexp);
    else {
        printf("error: expression incomplete.\n");
        err_sgn = 1;
    }

    free(mexp);
    return result;
}

/*
				match right bracket with left bracket
				return the length between the right one and the left one
 */

int Match(int lb_pos)  //       return the distance between two brackets
{
    int i, j, k, m;
    int lcount = 1, rcount = 0;;
    i = lb_pos;

    for (j = 0; j < BRKT_MAX; j++)
        if (stkr[j] > stkl[lb_pos]) 	break;

    m = stkr[j];

    while (stkr[j] > 0) {
        rcount++;

        for (k = i; ; k++) {
            if (stkl[k] < m) {
                if (lcount == rcount) {
                    if (stkl[k + 1] != 0 && stkl[k + 1] < stkr[j]) {
                        lcount = 1;
                        break;
                    }

                    return (stkr[j] - stkl[lb_pos] - 1);//find matching one's position
                }
            } else {
                lcount = 1;
                break;
            }

            lcount++;
        }

        j++;
    }

    return 0;
}

/*
		Core_Cmp(): finish calculating
 */

double Core_Cmp(EXP *expr)
{
    int i, j, t;

    if ((t = strlen(expr->chr)) != 0) {
        for (i = 0; i < t; i++) {
            if (expr->chr[i] == '^') {
                expr->num[i] = pow(expr->num[i], expr->num[i + 1]);

                for (j = i; j < t; j++) {
                    expr->num[j + 1] = expr->num[j + 2];
                    expr->chr[j] = expr->chr[j + 1];
                }

                t--;
                i = -1;
            }
        }

        for (i = 0; i < t; i++) {
            if (expr->chr[i] == '*') {
                expr->num[i] *= expr->num[i + 1];

                for (j = i; j < t; j++) {
                    expr->num[j + 1] = expr->num[j + 2];
                    expr->chr[j] = expr->chr[j + 1];
                }

                t--;
                i = -1;
            } else if (expr->chr[i] == '/') {
                expr->num[i] /= expr->num[i + 1];

                for (j = i; j < t; j++) {
                    expr->num[j + 1] = expr->num[j + 2];
                    expr->chr[j] = expr->chr[j + 1];
                }

                i = -1;
                t--;
            } else if (expr->chr[i] == '%') {
                if (IsInteger(expr->num[i]) && IsInteger(expr->num[i + 1])) {
                    expr->num[i] = (long long)expr->num[i] % (long long)expr->num[i + 1];

                    for (j = i; j < t; j++) {
                        expr->num[j + 1] = expr->num[j + 2];
                        expr->chr[j] = expr->chr[j + 1];
                    }

                    i = -1;
                    t--;
                } else {
                    printf("error: oprands of mod should be integer.\n");
                    err_sgn = 1;
                }
            }
        }

        for (i = 0; i < t; i++) {
            if (expr->chr[i] == '+') {
                expr->num[i] += expr->num[i + 1];

                for (j = i; j < t; j++) {
                    expr->num[j + 1] = expr->num[j + 2];
                    expr->chr[j] = expr->chr[j + 1];
                }

                i = -1;
                t--;
            } else if (expr->chr[i] == '-') {
                expr->num[i] -= expr->num[i + 1];

                for (j = i; j < t; j++) {
                    expr->num[j + 1] = expr->num[j + 2];
                    expr->chr[j] = expr->chr[j + 1];
                }

                i = -1;
                t--;
            }
        }
    }

    return expr->num[0];
}



void Reset(void)
{
    int i;

    for (i = 0; i < BRKT_MAX; i++) {
        stkl[i] = stkr[i] = 0;
    }

    lbrkt = 0;
    err_sgn = 0;
}


double StringToFloat(char *args, int *start)
{
    double t = 0;
    double xt = 0.1;
    int k = *start;
    int neg_token = 0;

    if (args[k] == '-') {
        neg_token = 1;
        k++;
    }

    for (; isdigit(args[k]); k++) {
        t *= 10;
        t += args[k] - '0';
    }

    if (args[k] == '.') {
        k++;

        while (isdigit(args[k])) {
            t += (args[k] - '0') * xt;
            xt /= 10;
            k++;
        }
    }

    if (args[k] == '.') {
        printf("error: wrong float number");
        err_sgn = 1;
        return 0;
    }

    *start = k - 1;

    if(neg_token)
        return -t;
    else
        return t;
}


double MacroReplace(char *args, int *start)
{
    int temp = (*start);
    int length = strlen(args);
    int i;
    int cnt_l = 0, cnt_r = 0;
    char *lp;
    int sl;
    char t[20];

    while (isalpha(args[(*start)]) && (*start) < length)
        t[(*start) - temp] = args[(*start)++];

    t[(*start) - temp] = '\0';

    (*start)--;

    if (args[(*start) + 1] == '(') {
        i = (*start)++;

        do {
            if (args[(*start)] == '(') cnt_l++;

            if (args[(*start)] == ')') cnt_r++;

            if ((*start) == length) {
                printf("error: square brackets don't match\n");
                err_sgn = 1;
                return 0;
            }

            (*start)++;
        } while (cnt_l != cnt_r);

        (*start)--;
        lp = args + i + 2;
        sl = (*start) - i - 2;

        if (i - temp == 2) {
            if      (StrMatch(args + temp, "sin")) return sin(SubCompute(lp, sl));
            else if (StrMatch(args + temp, "cos")) return cos(SubCompute(lp, sl));
            else if (StrMatch(args + temp, "tan")) return tan(SubCompute(lp, sl));
            else if (StrMatch(args + temp, "log")) return log(SubCompute(lp, sl));
        } else if (i - temp == 3) {
            if      (StrMatch(args + temp, "sinh")) return sinh(SubCompute(lp, sl));
            else if (StrMatch(args + temp, "cosh")) return cosh(SubCompute(lp, sl));
            else if (StrMatch(args + temp, "tanh")) return tanh(SubCompute(lp, sl));
            else if (StrMatch(args + temp, "asin")) return asin(SubCompute(lp, sl));
            else if (StrMatch(args + temp, "acos")) return acos(SubCompute(lp, sl));
            else if (StrMatch(args + temp, "atan")) return atan(SubCompute(lp, sl));
        } else if (i - temp == 4) {
            if      (StrMatch(args + temp, "asinh")) return asinh(SubCompute(lp, sl));
            else if (StrMatch(args + temp, "acosh")) return acosh(SubCompute(lp, sl));
            else if (StrMatch(args + temp, "atanh")) return atanh(SubCompute(lp, sl));
        } else {
            printf("error: function unidentified:\"");

            for (i = temp; i <= (*start); i++)
                printf("%c", args[i]);

            printf("\"\n");
            err_sgn = 1;
            return 0;
        }

    } else {

        if      (StrMatch(args + temp, "EL" ) && !isalpha(args[temp + 2])) return EL;
        else if (StrMatch(args + temp, "GM" ) && !isalpha(args[temp + 2])) return GM;
        else if (StrMatch(args + temp, "PI" ) && !isalpha(args[temp + 2])) return PI;
        else if (StrMatch(args + temp, "PHI") && !isalpha(args[temp + 3])) return PHI;
        else {
            int index = find_var(&table, t);
            struct var_pair *var;

            if(index != -1) {
                var = get_var(&table, index);

                (*start)++;

                while(isspace(args[((*start))])) ++(*start);

                if(args[*start] == '=') {
                    double *v = (double *)malloc(sizeof(double));
                    *v = SubCompute(args + (*start) + 1, length - (*start) - 1);
                    var->val.value = v;
                    (*start) = length;
                    return *v;
                } else {
                    (*start)--;

                    if(var->val.val_type == normal)
                        return *(double *)var->val.value;
                }

            } else {
                (*start)++;

                while(isspace(args[((*start))])) ++(*start);

                if(args[*start] == '=') {
                    for(int i = 0; i < BASE_SIZE; ++i) {
                        if(StrMatch(complete_list[i], t)) {
                            printf("error: \"%s\" is reserved, you shouldn't assign value for it\n", t);
                            err_sgn = 1;
                            return 0;
                        }
                    }

                    strcpy(table.contents[table.size].var_name, t);
#if defined __unix
                    complete_list[list_size++] = (char *)&table.contents[table.size].var_name;
#endif
                    table.contents[table.size].val.val_type = normal;
                    double *v = (double *)malloc(sizeof(double));
                    *v = SubCompute(args + (*start) + 1, length - (*start) - 1);
                    table.contents[table.size].val.value = v;
                    ++table.size;

                    if(table.size == table.max_size)
                        table.resize(table.this);

                    (*start) = length;
                    return *v;
                } else {
                    printf("error: input unidentified \"");

                    for (i = temp; i <= (*start); i++)
                        printf("%c", args[i]);

                    printf("\"\n");
                    err_sgn = 1;
                }
            }

            return 0;
        }
    }

    return 0;
}


int StrMatch(const char *arg_1, const char *arg_2) //Assistant function
{
    int len = strlen(arg_1);
	int len2 = strlen(arg_2);
	if(len != len2) return 0;
    char p1[len], p2[len];
    int i;

    for (i = 0; i < len; i++) {
        p1[i] = toupper(arg_1[i]);
        p2[i] = toupper(arg_2[i]);
    }

    for (i = 0; i < len; i++)
        if (p1[i] != p2[i])  return 0;

    return 1;
}


int IsInteger(double num)
{
    return ((long long)num - num) < 0.000000001 && (num - (long long)num) < 0.000000001 ? 1 : 0;
}


/********************_MATH_FUNCTION_START_***********************/








long long int Factorial(int input, int cas)
{
    long long int result = 1;

    if (cas) while (input > 0) {
            result *= input;
            input--;
        }
    else
        while (input > 0) {
            result *= input;
            input -= 2;
        }

    return result;
}





/*********************_VAR_TABLE_FUNCTION_END_************************/
int find_var(struct var_table *tp, char *name)
{
    for(int i = 0; i < tp->size; ++i)
        if(StrMatch(name, (tp->contents + i)->var_name))
            return i;

    return -1;
}
struct var_pair *get_var(struct var_table *tp, int index)
{
    return (tp->contents + index);
}

void double_size(void *tp)
{
    struct var_table *this = (struct var_table *)tp;
    struct var_pair *new_contents = (struct var_pair *)malloc(2 * this->max_size * sizeof(struct var_pair));
    memcpy(new_contents, this->contents, this->max_size * sizeof(struct var_pair));
    this->max_size *= 2;
    free(this->contents);
    this->contents = new_contents;
}
/*********************_VAR_TABLE_MODULE_START_************************/





/*******************^^*_MODE_MODULE_END_^^***********************/


void GetHelp(void)
{
    printf("Operators usable(rank by calculation priorities):\n\n\
               ( )   mod     !!         !       ^     \n\
            brackets mod factorial2 factorial power   \n\
                %%        *         /      +     -\n\
            GetAnswer multiply  divide  plus  minus\n");
    puts("");
    printf("Math functions usable: \n\
            sin cos tan asin acos atan \n\
            sinh cosh tanh asinh acosh atanh log\n");
    puts("");
    printf("Useful macros:\n\
            EL(e)  = %f  GM (Eular Gamma)  = %f\n\
            PI(Pi) = %f  PHI(golden ratio) = %f\n", EL, GM, PI, PHI);
    printf("Input form:\n\
            Input example:cos(2 ^ (1.23 + 5 mod 2) * PI) - 2 !\n\
            The computer ignores spaces, and is case-insensitive.\n");
    puts("");
    printf("ps:\n\
           1: Use \"%%num\" to get output[num]\n\
           2: Use brackets to demarcate exponent\n\
           3: support assignments, but name of variates should only consist of alphabets\n\
           4: function names are reserved \n\n");
}
void pFloat(double n)
{
	static char r[1024];
	sprintf(r,"%.12f",n);
	for(int i=strlen(r)-1;i>0;--i)
		if(r[i]!='0')
			if(r[i]=='.')
			{
				r[i]='\0';
				printf("%s",r);
				return;
			}
			else
			{
				r[i+1]='\0';
				printf("%s",r);
				return;
			}
}
