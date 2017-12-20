/**
 * @file mathformula.c
 * @brief 计算数学公式
 * @author pyq mail:pyqhaoku@163.com
 * @version v1.0
 * @date 2017-12-17
 */

#include <stdio.h>
#include <math.h>

#include <mathformula.h>

//operatorchar opchar;
//formuladata  fodata;

typedef double mfDescToValue_func(char *);

mfDescToValue_func *mfDescToValue = (mfDescToValue_func *)NULL;

int operatorcharPop(operatorchar *pchar, char ch)
{
	// malloc
	if(pchar->top == pchar->ocsize - 1)
	{
		pchar->opchar = realloc(pchar->ocsize * 2 * sizeof(char));
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
	if(top > 0)
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
	for( i = 0; i <= top; i++)
	{
		// check value == number  if not mfDescToValue_func changed
		fdata->formulaNumber[i] = mfDescToValue(fdata->formulaString[i]);
	}

	return 0;
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
			flag++;
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
	int indexstr = 0;
	
	initMathFormula();

	return 0;
}
