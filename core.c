#include"computer.h"
#include<stdio.h>
#include<string.h>
#include<stdlib.h>
#include<math.h>
#include<ctype.h>


extern double *Answers;
extern int Answer_Counter;
extern int err_sgn;


void Compute(char * args, int max)
{
    Reset();
    int i;
    for (lctl = 0, i = 0, rctl = 0; i < strlen(args); i++)
    {
        if (args[i] == '(')
            stkl[lctl++] = i;
        else if (args[i] == ')')
            stkr[rctl++] = i;
        if (rctl > lctl)
            break;
    }
    i = 0;
    if (lctl != rctl)
    {
        printf("Brackets don't match\n");
        return;
    }
    Answers[Answer_Counter - 1] = SubCompute(args, max);

    if (!err_sgn)
    {
        if(IsInteger(Answers[Answer_Counter - 1]))
            printf("Output[%d]:%lld\n", Answer_Counter, (long long)Answers[Answer_Counter - 1]);
        else
            printf("Output[%d]:%.12f\n", Answer_Counter, Answers[Answer_Counter - 1]);
        Answer_Counter++;
    }
    return;
}




double SubCompute(char * args, int max)

{
    int i, temp;
    int chr_ctl = 0, num_ctl = 0;
    double result = 0;
    EXP * mexp = (EXP *)malloc(sizeof(EXP));
    i = 0;
    if (StrMatch((args), "help") && strlen(args) == max)
    {
        GetHelp();
        err_sgn = 1;
        return 0;
    }
    if(args[i]=='-')
    {
        mexp->num[num_ctl++] = -1;
        mexp->chr[chr_ctl++] = '*';
        i++;
    }
    while (i < max)
    {
        if (isdigit(args[i]))
            mexp->num[num_ctl++] = StringToFloat(args, &i);
        else if (StrMatch(args + i, "mod"))
        {
            mexp->chr[chr_ctl++] = '%';
            i += 2;
        }
        else if (isalpha(args[i]))
            mexp->num[num_ctl++] = MacroReplace(args, &i);
        else
        {
            switch (args[i])
            {
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
                if (IsInteger(mexp->num[num_ctl - 1]))
                {
                    if(mexp->num[num_ctl - 1] < 0)
                    {
                        printf("The parameter of Factorial function should be a natural number.\n");
                        err_sgn = 1;
                        return 0;
                    }
                    if (args[i + 1] == '!')
                    {
                        i++;
                        mexp->num[num_ctl - 1] = (double)Factorial((int)mexp->num[num_ctl - 1], 0);
                    }
                    else
                        mexp->num[num_ctl - 1] = (double)Factorial((int)mexp->num[num_ctl - 1], 1);
                    break;
                }
                else
                {
                    printf("The parameter of Factorial function should be an integer.\n");
                    err_sgn = 1;
                    return 0;
                }
            case '^':
            {
                mexp->chr[chr_ctl++] = '^';
                if(args[i + 1]=='-')
                {
                    mexp->num[num_ctl++] = -1;
                    mexp->chr[chr_ctl++] = '*';
                    i += 1;
                }
                break;
            }
            case '%':
            {
                if(isdigit(args[i + 1]))
                    temp = StringToFloat(args + 1, &i);
                else
                    temp = Answer_Counter;
                i++;
                if (IsInteger((double)temp))
                {
                    if (temp >= Answer_Counter)
                    {
                        printf("usage:\"%%num\"  num:the number should be less than current order number\n");
                        err_sgn = 1;
                        return 0;
                    }
                    else
                    {
                        mexp->num[num_ctl++] = Answers[temp - 1];
                        break;
                    }
                }
                else
                {
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
    else
    {
        printf("error: expression incorrect.\n");
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
        if (stkr[j] > lb_pos) 	break;
    m = stkr[j];
    while (stkr[j] > 0)
    {
        rcount++;
        for (k = i; ; k++)
        {
            if (stkl[k] < m)
            {
                if (lcount == rcount)
                {
                    if (k + 1 == BRKT_MAX || stkl[k + 1] == 0)
                        return (stkr[j] - stkl[lb_pos] - 1);//find matching one's position
                    if (stkl[k + 1] < stkr[j])
                    {
                        lcount = 1;
                        break;
                    }
                }
            }
            else
            {
                lcount = 1;
                break;
            }
            lcount++;
        }
        j++;
    }
}

/*
		Core_Cmp(): finish calculating
 */

double Core_Cmp(EXP * expr)
{
    int i, j, t;
    if ((t = strlen(expr->chr)) != 0)
    {
        for (i = 0; i < t; i++)
        {
            if (expr->chr[i] == '^')
            {
                expr->num[i] = pow(expr->num[i], expr->num[i + 1]);
                for (j = i; j < t; j++)
                {
                    expr->num[j + 1] = expr->num[j + 2];
                    expr->chr[j] = expr->chr[j + 1];
                }
                t--;
                i = -1;
            }
        }
        for (i = 0; i < t; i++)
        {
            if (expr->chr[i] == '*')
            {
                expr->num[i] *= expr->num[i + 1];
                for (j = i; j < t; j++)
                {
                    expr->num[j + 1] = expr->num[j + 2];
                    expr->chr[j] = expr->chr[j + 1];
                }
                t--;
                i = -1;
            }
            else if (expr->chr[i] == '/')
            {
                expr->num[i] /= expr->num[i + 1];
                for (j = i; j < t; j++)
                {
                    expr->num[j + 1] = expr->num[j + 2];
                    expr->chr[j] = expr->chr[j + 1];
                }
                i = -1;
                t--;
            }
            else if (expr->chr[i] == '%')
            {
                if (IsInteger(expr->num[i]) && IsInteger(expr->num[i + 1]))
                {
                    expr->num[i] = (long long)expr->num[i] % (long long)expr->num[i + 1];
                    for (j = i; j < t; j++)
                    {
                        expr->num[j + 1] = expr->num[j + 2];
                        expr->chr[j] = expr->chr[j + 1];
                    }
                    i = -1;
                    t--;
                }
                else
                {
                    printf("error: oprands of mod should be integer.\n");
                    err_sgn = 1;
                }
            }
        }
        for (i = 0; i < t; i++)
        {
            if (expr->chr[i] == '+')
            {
                expr->num[i] += expr->num[i + 1];
                for (j = i; j < t; j++)
                {
                    expr->num[j + 1] = expr->num[j + 2];
                    expr->chr[j] = expr->chr[j + 1];
                }
                i = -1;
                t--;
            }
            else if (expr->chr[i] == '-')
            {
                expr->num[i] -= expr->num[i + 1];
                for (j = i; j < t; j++)
                {
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
    for (i = 0; i < BRKT_MAX; i++)
    {
        stkl[i] = stkr[i] = 0;
    }
    lbrkt = 0;
    err_sgn = 0;
}


double StringToFloat(char * args, int * start)
{
    double t = 0;
    double xt = 0.1;
    int k;
    for (k = *(start); isdigit(args[k]); k++)
    {
        t *= 10;
        t += args[k] - '0';
    }
    if (args[k] == '.')
    {
        k++;
        while (isdigit(args[k]))
        {
            t += (args[k] - '0') * xt;
            xt /= 10;
            k++;
        }
    }
    if (args[k] == '.')
    {
        printf("error: wrong form of float number");
        err_sgn = 1;
        return 0;
    }
    *start = k - 1;
    return t;
}


double MacroReplace(char * args, int * start)
{
    int temp = (*start);
    int length = strlen(args);
    int i;
    int cnt_l = 0, cnt_r = 0;
    while (isalpha(args[(*start) + 1]) && (*start) < length)
        (*start)++;
    if (args[(*start) + 1] == '[')
    {
        i = (*start);
        (*start)++;
        do
        {
            if (args[(*start)] == '[') cnt_l++;
            if (args[(*start)] == ']') cnt_r++;
            if ((*start) == length)
            {
                printf("error: square brackets don't match\n");
                err_sgn = 1;
                return 0;
            }
            (*start)++;
        }
        while (cnt_l != cnt_r);
        (*start)--;
        if (i - temp == 2)
        {
            if      (StrMatch(args + temp, "sin")) return sin(SubCompute(args + i + 2, (*start) - i - 2));
            else if (StrMatch(args + temp, "cos")) return cos(SubCompute(args + i + 2, (*start) - i - 2));
            else if (StrMatch(args + temp, "tan")) return tan(SubCompute(args + i + 2, (*start) - i - 2));
            else if (StrMatch(args + temp, "log")) return log(SubCompute(args + i + 2, (*start) - i - 2));
        }
        else if (i - temp == 3)
        {
            if      (StrMatch(args + temp, "sinh")) return sinh(SubCompute(args + i + 2, (*start) - i - 2));
            else if (StrMatch(args + temp, "cosh")) return cosh(SubCompute(args + i + 2, (*start) - i - 2));
            else if (StrMatch(args + temp, "tanh")) return tanh(SubCompute(args + i + 2, (*start) - i - 2));
            else if (StrMatch(args + temp, "asin")) return asin(SubCompute(args + i + 2, (*start) - i - 2));
            else if (StrMatch(args + temp, "acos")) return acos(SubCompute(args + i + 2, (*start) - i - 2));
            else if (StrMatch(args + temp, "atan")) return atan(SubCompute(args + i + 2, (*start) - i - 2));
        }
        else if (i - temp == 4)
        {
            if      (StrMatch(args + temp, "asinh")) return asinh(SubCompute(args + i + 2, (*start) - i - 2));
            else if (StrMatch(args + temp, "acosh")) return acosh(SubCompute(args + i + 2, (*start) - i - 2));
            else if (StrMatch(args + temp, "atanh")) return atanh(SubCompute(args + i + 2, (*start) - i - 2));
        }
        else
        {
            printf("error: function unidentified:\"");
            for (i = temp; i <= (*start); i++)
                printf("%c", args[i]);
            printf("\"\n");
            err_sgn = 1;
            return 0;
        }
    }
    else
    {
        if      (StrMatch(args + temp, "EL" ) && !isalpha(args[temp + 2])) return EL;
        else if (StrMatch(args + temp, "GM" ) && !isalpha(args[temp + 2])) return GM;
        else if (StrMatch(args + temp, "PI" ) && !isalpha(args[temp + 2])) return PI;
        else if (StrMatch(args + temp, "PHI") && !isalpha(args[temp + 3])) return PHI;
        else
        {

            printf("error: input unidentified \"");
            for (i = temp; i <= (*start); i++)
                printf("%c", args[i]);
            printf("\"\n");
            err_sgn = 1;
            return 0;
        }
    }

}


int StrMatch(const char * arg_1, const char * arg_2) //Assistant function
{
    int len = strlen(arg_2);
    char p1[len], p2[len];
    int i;
    for (i = 0; i < len; i++)
    {
        p1[i] = toupper(arg_1[i]);
        p2[i] = toupper(arg_2[i]);
    }
    for (i = 0; i < len; i++)
        if (p1[i] != p2[i])  return 0;
    return 1;
}


int IsInteger(double num)
{
    return ((long long)num - num)<0.000000001 && (num - (long long)num) < 0.000000001?1:0;
}


/********************_MATH_FUNCTION_START_***********************/








long long int Factorial(int input, int cas)
{
    long long int result = 1;
    if (cas) while (input > 0)
        {
            result *= input;
            input--;
        }
    else
        while (input > 0)
        {
            result *= input;
            input -= 2;
        }
    return result;
}





/*********************_MATH_FUNCTION_END_************************/

void ModeIn(char * args)
{
    return;
}


/*********************_MODE_MODULE_START_************************/





/*******************^^*_MODE_MODULE_END_^^***********************/


void GetHelp(void)
{
    printf("Operators usable(rank by count priorities):\n\n\
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
            Input example:cos[2 ^ (1.23 + 5 mod 2) * PI] - 2 !\n\
            The computer ignores spaces, and is case-insensitive.\n");
    puts("");
    printf("Tips:\n\
           1: Use \"%%num\" to get output[num]\n\
           2: Use brackets to demarcate exponent\n\n");
}
