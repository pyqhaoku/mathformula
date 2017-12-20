/**
 * @file mathformula.c
 * @brief 计算数学公式
 * @author pyq mail:pyqhaoku@163.com
 * @version v1.0
 * @date 2017-12-17
 */

#include <stdio.h>
#include <math.h>
#include <stdlib.h>
#include <string.h>

#include <mathformula.h>
#include <pstring.h>

#define strdup(a) strcpy_p(a)

//operatorchar opchar;
//formuladata  fodata;
//
operatorList ol[OL_MAX_SIZE];

typedef double mfDescToValue_func(char *);
typedef double mfCalc_func(int num,...);

mfDescToValue_func *mfDescToValue = (mfDescToValue_func *)NULL;

int operatorListAdd(char *string, int level, int number, int type, double(*mfCalc_func)(int num,...))
{
	int index = 0;
	for(index = 0; index < OL_MAX_SIZE; index++)
	{
		if(ol[index].string == NULL)
		{
			ol[index].id = index;
			ol[index].string = strdup(string);
			ol[index].level = level;
			ol[index].number = number;
			ol[index].type = type;
			ol[index].mfCalc_func = mfCalc_func;

			break;
		}
	}

	return index;
}

int operatorcharPop(operatorchar *pchar, char ch)
{
	// malloc
	if(pchar->top == pchar->ocsize - 1)
	{
		pchar->opchar = realloc(pchar->opchar, pchar->ocsize * 2 * sizeof(char));
		pchar->ocsize *= 2;
	}

	pchar->top ++;
	pchar->opchar[pchar->top] = ch;
	return 0;
}

int operatorcharPush(operatorchar *pchar)
{
	if(pchar->top < 0)
	{
		return 0xfaaa;
	}

	return pchar->opchar[pchar->top--];
}

int operatorcharIsEmpty(operatorchar *pchar)
{
	if(pchar->top < 0)
	{
		return 1;
	}

	return 0;
}

int formuladataPop(formuladata *fdata, char *desc)
{
	if(fdata->top == fdata->fdsize - 1)
	{
		fdata->formulaString = realloc(fdata->formulaString, fdata->fdsize * 2 * sizeof(char *));
		fdata->formulaNumber = realloc(fdata->formulaNumber, fdata->fdsize * 2 * sizeof(double));
		fdata->fdsize *= 2;
	}

	fdata->top ++;
	fdata->formulaString[fdata->top] = strcpy_p(desc);

	return 0;
}

double formuladataPush(formuladata *fdata)
{
	if(fdata->top > 0)
	return fdata->formulaNumber[fdata->top--];
	else
	{
		/// 随便写的数字
		return 0xfaaa;
	}
}

int formuladataIsEmpty(formuladata *fdata)
{
	if(fdata->top < 0)
	{
		return 1;
	}

	return 0;
}

int formuladataDispel(formuladata *fdata)
{
	int i;
	for( i = 0; i <= fdata->top; i++)
	{
		// check value == number  if not mfDescToValue_func changed
		fdata->formulaNumber[i] = mfDescToValue(fdata->formulaString[i]);
	}

	return 0;
}

/* ----------------------------------------------------------------------------*/
/**
 * @brief 检查括号匹配 以及 运算符判断
 *
 * @param restr
 * @param start 括号起始位置 或 字符串起始位置
 * @param rsize 字符串大小
 *
 * @return 
 */
/* ----------------------------------------------------------------------------*/
int analysis_parenthesis(char *restr, int start, int rsize)
{
	int len = strlen(restr);
	if(start >= len - 1 || start >= rsize - 1) return 0;

	// set ' ' to '#'
	int flag = 0;
	if(start == 0)
	{
		flag = 1;
	}
	else if(restr[start-1] == DELIMITER)
	{
		flag = 1;
	}

	int s = start + 1;
	while(s < len  && s < rsize)
	{
		if( flag && restr[s] == ' ')
		{
			restr[s] = DELIMITER;
		}
		else if(restr[s] == ')')
		{
			if(flag)
			{
				restr[s+1] = DELIMITER;
			}

			if(start == 0)
			{
				fprintf(stderr, "analysis_parenthesis(%s) failed\n", restr);
				return -1;
			}

			// skip ' ) '
			return s - start + 1;
		}
		else if(restr[s] == '(')
		{
			int rc = analysis_parenthesis(restr, s, rsize);
			if(rc < 0)
			{
				// failed
				return -1;
			}
			s += rc;
		}

		s++;
	}

	if(start != 0)
	{
		fprintf(stderr, "analysis_parenthesis(%s) failed\n", restr);
		return -1;
	}
	return 0;
}

char *analysis(char *str, int strsize)
{
	char *restr = malloc(strsize * 3);
	strcpy(restr, str);

	stringRemoveSpace(restr);

	int index = 0, olnum = 0;
	// type 1
	for(index =0 ; index < OL_MAX_SIZE; index++)
	{
		if(ol[index].string == NULL)
		{
			break;
		}

		if(ol[index].type == 2)
		{
			// skip
			continue;
		}

		olnum++;
		int len = strlen(ol[index].string);
		char *temp = malloc(len + 3);
		snprintf(temp, len + 3, " %s ", ol[index].string);
		replace(restr, strsize * 3, ol[index].string, temp);

		free(temp);
	}

	for(index = 0; index < OL_MAX_SIZE; index++)
	{
		if(ol[index].string == NULL)
		{
			break;
		}

		if(ol[index].type == 1)
		{
			// skip
			continue;
		}

		olnum++;
		int len = strlen(ol[index].string);
		char *temp = malloc(len + 3);
		snprintf(temp, len + 3, " %s(", ol[index].string);

		char *temp2 = malloc(len + 7);
		snprintf(temp2, len + 7, " #%s##(#", ol[index].string);
		
		replace(restr, strsize * 3, temp, temp2);

	}

	replace(restr, strsize * 3, ")", " ) ");

	// deal ()
	int rc = analysis_parenthesis(restr, 0, strsize * 3);

	// clear space
	stringRemoveSpace(restr);
	if(rc < 0)
	{
		restr = NULL;
		free(restr);
	}
	return restr;
}

int readPChar(char str[], int sindex, int strsize)
{
	while(sindex < strsize && str[sindex] == ' ') sindex++;
	int eindex = sindex + 1;
	
}

int readFData(char str[], int sindex, int strsize)
{
	// number  or string
	while(sindex < strsize && str[sindex] == ' ') sindex++;
	int eindex = sindex + 1;

	// parentheses number
	int pflag = 0;
	while(eindex < strsize)
	{
		if(str[eindex] == '(')
		{
			pflag++;
		}
		else if(pflag > 0)
		{
			if(str[eindex] == ')')
			{
				pflag--;
			}
		}
		else
		{
			// change on feather
			if(str[eindex] == '+' || str[eindex] == '-' ||
					str[eindex] == '*' || str[eindex] == '/' ||
					str[eindex] == '^')
			{
				break;
			}
		}
		eindex++;
	}

	return eindex;
}

int mfTransformation(char str[], int strsize)
{
	char ch;
	int index = 0;

	operatorchar pchar;
	formuladata fdata;

	operatorcharInit(&pchar);
	formuladataInit(&fdata);

	while(index < strsize)
	{
		if(str[index] == '#')
		{
			// operatorchar
			int len = 0;
			char *s = strtagcpy(str + index + 1, "#", &len);
			// skip "#s#"
			index += len + 2;
			if(strcmp(s, "(") == 0)
			{
				operatorcharPop(&pchar, 0);
				
			}
			else if(strcmp(s, ")") == 0)
			{
			}
			else
			{

			}
		}
	}
	

	return 0;
}

/// check function 
int main()
{
	operatorListAdd("(", 0, 0, 0, NULL);
	operatorListAdd("+", 1, 2, 1, NULL);
	operatorListAdd("-", 1, 2, 1, NULL);
	operatorListAdd("*", 2, 2, 1, NULL);
	operatorListAdd("/", 2, 2, 1, NULL);

	//char test[] = "abc(1+2)+log(2+3)";
	char test[256];
	while(~scanf("%s", test))
	{
		char *p = analysis(test, strlen(test));
		printf("test=|%s|\n", p);
		free(p);
		//mfTransformation(p, strlen(p));
	}

	return 0;
}
