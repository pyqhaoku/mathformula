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

#ifndef strdup
#define strdup(a) strcpy_p(a)
#endif

//operatorchar opchar;
//formuladata  fodata;
//
operatorList ol[OL_MAX_SIZE];

typedef int mfDescToValue_func(char *desc, double *data);
typedef int mfCalc_func(int num, double data[], double *result);

mfDescToValue_func *mfDescToValue = (mfDescToValue_func *)NULL;

int operatorListAdd(char *string, int level, int number, int type, int (*mfCalc_func)(int num,double data[], double *result))
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

int operatorcharInit(operatorchar *pchar)
{
	memset(pchar, 0, sizeof(operatorchar));

	pchar->ocsize = 10;
	pchar->opchar = malloc(10 * sizeof(char));
	pchar->top = -1;

	return 0;
}

int operatorcharFree(operatorchar *pchar)
{
	free(pchar->opchar);
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
	if(mfDescToValue != NULL && isdigit(desc[0]) == 0)
	{
		double number;
		int rc = mfDescToValue(desc, &number);
		if(rc < 0)
		{
			fprintf(stderr, "mfDescToValue(%s) failed\n", desc);
			return -1;
		}

		fdata->formulaNumber[fdata->top] = number;
	}
	else
	{
		double number = atof(desc);
		if(number == 0 && strncmp(desc, "0", 1) != 0)
		{
			fprintf(stderr, "%s not number\n", desc);
			return -1;
		}

		fdata->formulaNumber[fdata->top] = number;
	}

	return 0;
}

int formuladataPopNumber(formuladata *fdata, double number)
{
	if(fdata->top == fdata->fdsize - 1)
	{
		fdata->formulaString = realloc(fdata->formulaString, fdata->fdsize * 2 * sizeof(char *));
		fdata->formulaNumber = realloc(fdata->formulaNumber, fdata->fdsize * 2 * sizeof(double));
		fdata->fdsize *= 2;
	}

	fdata->top ++;
	fdata->formulaString[fdata->top] = NULL;
	fdata->formulaNumber[fdata->top] = number;

	return 0;
}

double formuladataPush(formuladata *fdata)
{
	if(fdata->top >= 0)
	{
		if(fdata->formulaString[fdata->top] != NULL)
		{
			free(fdata->formulaString[fdata->top]);
			fdata->formulaString[fdata->top] = NULL;
		}
		return fdata->formulaNumber[fdata->top--];
	}
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
	return 0;
	int i;
	for( i = 0; i <= fdata->top; i++)
	{
		// check value == number  if not mfDescToValue_func changed
		//fdata->formulaNumber[i] = mfDescToValue(fdata->formulaString[i]);
	}

	return 0;
}

int formuladataInit(formuladata *fdata)
{
	memset(fdata, 0, sizeof(formuladata));
	fdata->fdsize = 10;
	fdata->top = -1;
	fdata->formulaString = malloc(10 * sizeof(char *));
	fdata->formulaNumber = malloc(10 * sizeof(double));

	memset(fdata->formulaString, 0, sizeof(char *) * 10);
	return 0;
}

int formuladataFree(formuladata *fdata)
{
	free(fdata->formulaString);
	free(fdata->formulaNumber);
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

	//printf("debug:start=%d,restr[]=%c\n", start, restr[start]);
	// set ' ' to '#'
	int flag = 0;
	if(start == 0)
	{
		flag = 1;
	}
	else if(restr[start-2] == DELIMITER)
	{
		flag = 1;
	}

	int s = start;
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
			if(s > 1 && restr[s-2] != DELIMITER)
			{
				restr[s-1] = ' ';
			}
			int rc = analysis_parenthesis(restr, s + 1, rsize);
			if(rc < 0)
			{
				// failed
				return -1;
			}
			s += rc + 1;
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

	//printf("debug:restr=|%s|\n", restr);

	for(index = 0; index < OL_MAX_SIZE; index++)
	{
		if(ol[index].string == NULL)
		{
			break;
		}

		if(ol[index].type != 2)
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

	//printf("debug:restr=|%s|\n", restr);
	replace(restr, strsize * 3, ")", " ) ");

	//printf("debug:restr=|%s|\n", restr);
	// deal ()
	int rc = analysis_parenthesis(restr, 0, strsize * 3);

	//printf("debug:restr=|%s|\n", restr);
	// clear space
	stringRemoveSpace(restr);
	//printf("debug:restr=|%s|\n", restr);
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

int mfClacStack(operatorchar *pchar, formuladata *fdata, int level)
{
	do{
		if(operatorcharIsEmpty(pchar) == 1)
		{
			if(level == 0)
			{
				// )多余的右括号
				fprintf(stderr, "bugtag:leave)\n");
				return -1;
			}
			break;
		}
		int oindex = operatorcharPush(pchar);
		if(ol[oindex].level < level)
		{
			// pop
			operatorcharPop(pchar, oindex);
			break;
		}

		if(level == 0 && ol[oindex].level == 0)
		{
			break;
		}

		if(level == -1 && ol[oindex].level == 0)
		{
			// ( 多余的左括号
			fprintf(stderr, "bugtag:leave (\n");
			return -1;
		}

		int onumber = ol[oindex].number;
		double *data = malloc(onumber * sizeof(double));

		int i = 0;
		for( i = 0 ; i < onumber; i++)
		{
			if(formuladataIsEmpty(fdata) == 1)
			{
				fprintf(stderr, "bug formuladataIsEmpty()\n");
				return -1;
			}
			data[i] = formuladataPush(fdata);
		}

		double va = 0;
		int rc = ol[oindex].mfCalc_func(onumber, data, &va);
		free(data);

		if(rc < 0)
		{
			fprintf(stderr, "mfCalc_func() failed\n");
			return -1;
		}

		formuladataPopNumber(fdata, va);

	}while(1);

	return 0;
}

int mfTransformation(char str[], int strsize, double *result)
{
	int index = 0;
	int rc = 0;

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
				// ()
				rc = mfClacStack(&pchar, &fdata, 0);
				if(rc < 0)
				{
					fprintf(stderr, "mfClacStack() failed\n");
					goto EXIT_ERR;
				}

			}
			else
			{
				int oindex = 1;
				for( oindex = 1; oindex < OL_MAX_SIZE; oindex++)
				{
					if(ol[oindex].string == NULL)
					{
						break;
					}
					if(strcmp(s, ol[oindex].string) == 0)
					{
						break;
					}
				}

				if(oindex >= OL_MAX_SIZE || ol[oindex].string == NULL)
				{
					free(s);
					fprintf(stderr, "%s not define\n", ol[oindex].string);

					goto EXIT_ERR;
				}

				if(ol[oindex].mfCalc_func == NULL)
				{
					free(s);
					fprintf(stderr, "%s function not define\n", ol[oindex].string);

					goto EXIT_ERR;
				}

				int olevel = ol[oindex].level;
				rc = mfClacStack(&pchar, &fdata, olevel);
				if(rc < 0)
				{
					fprintf(stderr, "mfClacStack() failed\n");
					goto EXIT_ERR;
				}

				operatorcharPop(&pchar, oindex);
			}

			free(s);
		}
		else
		{
			// data
			int len = 0;
			char *numstr = strtagcpy(str + index, "#", &len);
			index += len;

			rc = formuladataPop(&fdata, numstr);
			free(numstr);
			if(rc < 0)
			{
				fprintf(stderr, "formuladataPop() failed\n");
				goto EXIT_ERR;
			}
		}
	}
	
	rc = mfClacStack(&pchar, &fdata, -1);
	if(rc < 0)
	{
		fprintf(stderr, "mfClacStack() failed\n");
		goto EXIT_ERR;
	}

	*result = formuladataPush(&fdata);

	operatorcharFree(&pchar);
	formuladataFree(&fdata);

	return 0;
EXIT_ERR:
	operatorcharFree(&pchar);
	formuladataFree(&fdata);

	return -1;
}

int mfSum(int num, double data[], double *result)
{
	*result = data[0] + data[1];
	printf("testtag(%s):data[1]=%lf data[0]=%lf\n", __FUNCTION__, data[1], data[0]);
	return 0;
}

int mfSub(int num, double data[], double *result)
{
	*result = data[1] - data[0];
	printf("testtag(%s):data[1]=%lf data[0]=%lf\n", __FUNCTION__, data[1], data[0]);
	return 0;
}

int mfMul(int num, double data[], double *result)
{
	*result = data[1] * data[0];
	printf("testtag(%s):data[1]=%lf data[0]=%lf\n", __FUNCTION__, data[1], data[0]);
	return 0;
}

int mfDiv(int num, double data[], double *result)
{
	if(data[0] == 0)
	{
		fprintf(stderr, "divide zero\n");
		return -1;
	}

	*result = data[1] / data[0];
	printf("testtag(%s):data[1]=%lf data[0]=%lf\n", __FUNCTION__, data[1], data[0]);
	return 0;
}

int mfDescToValue2(char *desc, double *data)
{
	*data = 0;
	int i = 0;
	int len = strlen(desc);
	for( i = 0 ; i < len ; i++)
	{
		*data += desc[i];
	}

	*data /= len;
	return 0;
}

/// check function 
int main()
{
	operatorListAdd("(", 0, 0, 0, NULL);
	operatorListAdd("+", 1, 2, 1, mfSum);
	operatorListAdd("-", 1, 2, 1, mfSub);
	operatorListAdd("*", 2, 2, 1, mfMul);
	operatorListAdd("/", 2, 2, 1, mfDiv);

	mfDescToValue = mfDescToValue2;

	//char test[] = "abc(1+2)+log(2+3)";
	char test[256];
	while(~scanf("%s", test))
	{
		char *p = analysis(test, strlen(test));
		printf("test=|%s|\n", p);
		double result = 0;
		mfTransformation(p, strlen(p), &result);
		free(p);
		printf("result = %lf\n", result);
	}

	return 0;
}
