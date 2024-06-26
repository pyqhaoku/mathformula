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

//#define __MF_DEBUG_MODE_

#define DEBUGARGS __FILE__,__LINE__,__FUNCTION__
#define DEBUGFMT  "[mathformula]%s(%d)-%s:"

#define _WriteLog(fmt,...) \
	do\
	{\
		printf(DEBUGFMT fmt, DEBUGARGS, ##__VA_ARGS__);\
	}while(0);

//operatorchar opchar;
//formuladata  fodata;
//
static operatorList ol[OL_MAX_SIZE];

mfDescToValue_func *mfDescToValue = (mfDescToValue_func *)NULL;
//mfDescToValue = (mfDescToValue_func *)NULL;
static int initflag = 0;

/* ----------------------------------------------------------------------------*/
/**
 * @brief 添加一个运算符
 *
 * @param string
 * @param level
 * @param number
 * @param type
 * @param mfCalc_func
 *
 * @return 
 */
/* ----------------------------------------------------------------------------*/
int operatorListAdd(char *string, int level, int number, int type, int (*mfCalc_func)(int num,double data[], double *result))
{
	if(initflag == 0)
	{
		mathformulaInit();
	}

	if(level < LEVEL_ZERO || level > LEVEL_TEN)
	{
		fprintf(stderr, "invalid level %d\n", level);
		return -1;
	}

	if(type != MATH_TYPE_A && type != MATH_TYPE_B && type != MATH_TYPE_Z)
	{
		fprintf(stderr, "invalid type %d\n", type);
		return -1;
	}

	if(type == MATH_TYPE_A && number != 2)
	{
		fprintf(stderr, "TYPE_A number must be two\n");
		return -1;
	}

	if(type == MATH_TYPE_B && level <= 3)
	{
		fprintf(stderr, "TYPE_B level must greater than 3\n");
		return -1;
	}

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

/* ----------------------------------------------------------------------------*/
/**
 * @brief 运算符入栈 内部函数
 *
 * @param pchar
 * @param ch
 *
 * @return 
 */
/* ----------------------------------------------------------------------------*/
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

/* ----------------------------------------------------------------------------*/
/**
 * @brief 运算符出栈 内部函数
 *
 * @param pchar
 *
 * @return 
 */
/* ----------------------------------------------------------------------------*/
int operatorcharPush(operatorchar *pchar)
{
	if(pchar->top < 0)
	{
		return 0xfaaa;
	}

	return pchar->opchar[pchar->top--];
}

/* ----------------------------------------------------------------------------*/
/**
 * @brief 判断运算符栈是否为空栈
 *
 * @param pchar
 *
 * @return 
 */
/* ----------------------------------------------------------------------------*/
int operatorcharIsEmpty(operatorchar *pchar)
{
	if(pchar->top < 0)
	{
		return 1;
	}

	return 0;
}

/* ----------------------------------------------------------------------------*/
/**
 * @brief 运算符栈初始化
 *
 * @param pchar
 *
 * @return 
 */
/* ----------------------------------------------------------------------------*/
int operatorcharInit(operatorchar *pchar)
{
	memset(pchar, 0, sizeof(operatorchar));

	pchar->ocsize = 10;
	pchar->opchar = malloc(10 * sizeof(char));
	pchar->top = -1;

	return 0;
}

/* ----------------------------------------------------------------------------*/
/**
 * @brief 运算符栈内存释放
 *
 * @param pchar
 *
 * @return 
 */
/* ----------------------------------------------------------------------------*/
int operatorcharFree(operatorchar *pchar)
{
	free(pchar->opchar);
	return 0;
}

/* ----------------------------------------------------------------------------*/
/**
 * @brief 计算数字入栈 内部函数
 *
 * @param fdata
 * @param desc
 *
 * @return 
 */
/* ----------------------------------------------------------------------------*/
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
	if(mfDescToValue != NULL && (isdigit(desc[0]) == 0 && desc[0] != '+' && desc[0] != '-'))
	{
		double number;
		int rc = mfDescToValue(desc, &number);
		if(rc < 0)
		{
			fprintf(stderr, "mfDescToValue(%s) failed\n", desc);
			return -1;
		}

		fdata->formulaNumber[fdata->top] = number;
#ifdef __MF_DEBUG_MODE_
		_WriteLog("desc=|%s| number=%lf\n", desc, number);
#endif
	}
	else
	{
		char *p = NULL;
		double number = strtof(desc, &p);
		if( p - desc - strlen(desc) == 0)
		{
			fdata->formulaNumber[fdata->top] = number;
		}
		//if(number == 0 && strncmp(desc, "0", 1) != 0)
		else
		{
			fprintf(stderr, "%s not number\n", desc);
			return -1;
		}
#ifdef __MF_DEBUG_MODE_
		_WriteLog("desc=|%s| number=%lf\n", desc, number);
#endif
	}

	return 0;
}

/* ----------------------------------------------------------------------------*/
/**
 * @brief 计算数字入栈 内部函数
 *
 * @param fdata
 * @param number
 *
 * @return 
 */
/* ----------------------------------------------------------------------------*/
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

/* ----------------------------------------------------------------------------*/
/**
 * @brief 计算数字出栈 内部函数
 *
 * @param fdata
 *
 * @return 
 */
/* ----------------------------------------------------------------------------*/
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

/* ----------------------------------------------------------------------------*/
/**
 * @brief 计算数字栈是否为空
 *
 * @param fdata
 *
 * @return 
 */
/* ----------------------------------------------------------------------------*/
int formuladataIsEmpty(formuladata *fdata)
{
	if(fdata->top < 0)
	{
		return 1;
	}

	return 0;
}

/* ----------------------------------------------------------------------------*/
/**
 * @brief 将计算数字栈中的特殊符号转化为数字 --弃用
 *
 * @param fdata
 *
 * @return 
 */
/* ----------------------------------------------------------------------------*/
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

/* ----------------------------------------------------------------------------*/
/**
 * @brief 计算数字栈初始化  内部函数
 *
 * @param fdata
 *
 * @return 
 */
/* ----------------------------------------------------------------------------*/
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

/* ----------------------------------------------------------------------------*/
/**
 * @brief 计算数字栈内存释放
 *
 * @param fdata
 *
 * @return 
 */
/* ----------------------------------------------------------------------------*/
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

#ifdef __MF_DEBUG_MODE_
	_WriteLog("debug:start=%d,restr[]=%c\n", start, restr[start]);
#endif
	// set ' ' to '#'
	int flag = 0;
	if(start == 1)
	{
		flag = 1;
	}
	else if(restr[start-2] == MATH_DELIMCHAR)
	{
		flag = 1;
	}

	int s = start;
	while(s < len  && s < rsize)
	{
		if( flag && restr[s] == ' ')
		{
			restr[s] = MATH_DELIMCHAR;
		}
		else if(restr[s] == '+' || restr[s] == '-')
		{
			if(s == 2)
			{
				restr[s-1] = ' ';
				restr[s+1] = ' ';
				s += 1;
			}
			else if(s > 2 && (restr[s-2] == MATH_DELIMCHAR || restr[s-2] == ' ') && restr[s-3] != ')')
			{
				// +/- 前面如果是运算符 则自身作正负号 如果不是 或者是右括号 则作加减号处理
				restr[s-1] = ' ';
				restr[s+1] = ' ';
				s += 1;
			}
		}
		else if(restr[s] == ')')
		{
			if(flag)
			{
				restr[s+1] = MATH_DELIMCHAR;
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
			if(s > 1 && restr[s-2] != MATH_DELIMCHAR)
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

	if(start != 1)
	{
		fprintf(stderr, "analysis_parenthesis(%s) failed\n", restr);
		return -1;
	}
	return 0;
}

/* ----------------------------------------------------------------------------*/
/**
 * @brief 计算公式识别
 * @note 将运算符号和数字用分隔符分开 方便计算处理
 * 不识别的符号会当做数字处理  如果外部提供的函数无法处理 则计算失败
 *
 * @param str
 * @param strsize
 *
 * @return 
 */
/* ----------------------------------------------------------------------------*/
char *analysis(const char *str, int strsize)
{
	char *restr = malloc(strsize * 3);
	char *strbk = strdup(str);
	memset(restr, 0, strsize * 3);

	stringRemoveSpace(strbk);
	int i= 0, flag = 0; // flag 0 ParserNumber  1 ParserOther ...
	int pflag = 0; // 
	int rl = 0;
	int eflag = 0;

	if(strbk[0] == '(')
	{
		flag = 1;
	}
	for(i = 0; i < strsize; i++)
	{
		char ch1 = strbk[i];

		if(flag == 0)
		{
			// Number of TYPE_B
			if(isalpha(ch1))
			{
				int ei = i + 1;
				while(isalpha(strbk[ei])) ei++;
				if(strbk[ei] == '(')
				{
					char *funcname = strncpy_p(strbk + i, ei - i);
					int findex = 0;
					for(findex = 0; findex < OL_MAX_SIZE; findex++)
					{
						if(ol[findex].string == NULL) break;
						if(ol[findex].type != MATH_TYPE_B) continue;
						if(strcmp(ol[findex].string, funcname) == 0)
							break;
					}

					if(findex < OL_MAX_SIZE && ol[findex].type == MATH_TYPE_B)
					{
						strcat(restr, MATH_DELIMITER);
						strcat(restr, funcname);
						char temp[20];
						snprintf(temp, sizeof(temp), "%c%c(%c", MATH_DELIMCHAR, MATH_DELIMCHAR, MATH_DELIMCHAR);
						strcat(restr, temp);
						rl = strlen(restr);
 
						pflag++;
						flag = 0;
						free(funcname);
						i = ei;

#ifdef __MF_DEBUG_MODE_
						printf("restr =  |%s|\n", restr);
#endif
						// check number of fun
						int aflag = 1;
						int kflag = ol[findex].number;
						while(aflag > 0 && ei < strsize)
						{
							ei++;
							if(strbk[ei] == '(') aflag++;
							else if(strbk[ei] == ')') aflag--;
							else if(strbk[ei] == ',') strbk[ei] = MATH_DELIMCHAR, kflag--;
						}

						if(kflag != 1 || aflag > 0)
						{
							// error
							eflag = 1;
							fprintf(stderr, "kflag = %d, aflag = %d\n", kflag, aflag);
							break;
						}

						continue;
					}
				}

				while(isalnum(strbk[ei]) || strbk[ei] == '.' || strbk[ei] == '_') ei++;
				if(strbk[ei] == '(')
				{
					// TEST(xxx)
					int aflag = 1;
					while(aflag > 0 && ei < strsize)
					{
						if(strbk[ei] == '(') aflag++;
						else if(strbk[ei] == ')') aflag--;
						ei++;
					}

					if(aflag > 0)
					{
						// error
						break;
					}

				}
				else if(strbk[ei] == '[')
				{
					// TEST[a,b]
					while(ei < strsize && strbk[ei] != ']') ei++;
					if(ei >= strsize)
					{
						// error
						break;
					}
					ei++;
				}

				while(i < ei)restr[rl++] = strbk[i];
#ifdef __MF_DEBUG_MODE_
				printf("restr =  |%s|\n", restr);
#endif

				flag = 1;
			}
			else
			{
				// number
				char *p = NULL;
				double value = strtof(strbk + i, &p);
				if( p - strbk - i > 0)
				{
					while(strbk + i < p)restr[rl++] = strbk[i++];
					i--;
					flag = 1;
#ifdef __MF_DEBUG_MODE_
					printf("restr =  |%s|\n", restr);
#endif
				}
				else
				{
					// error
					eflag = 1;
					break;
				}
			}
		}
		else if(flag == 1)
		{
			flag = (strbk[i+1] == '(' || strbk[i+1] == ')') ? 1 : 0;
			switch(strbk[i])
			{
				case '+':
				case '-':
				case '*':
				case '/':
						 restr[rl++] = MATH_DELIMCHAR;
						 restr[rl++] = strbk[i]; 
						 restr[rl++] = MATH_DELIMCHAR;
						 break;
				case ')':pflag--;
						 if(pflag < 0)
						 {
							 eflag = 1;
							 break;
						 }
						 restr[rl++] = MATH_DELIMCHAR;
						 restr[rl++] = strbk[i]; 
						 restr[rl++] = MATH_DELIMCHAR;
						 break;

				case '(':pflag++;
						 restr[rl++] = MATH_DELIMCHAR;
						 restr[rl++] = strbk[i]; 
						 restr[rl++] = MATH_DELIMCHAR;
						 break;
				case '>':
				case '<':
						 restr[rl++] = MATH_DELIMCHAR;
						 restr[rl++] = strbk[i]; 
						 if(strbk[i+1] == '=') { restr[rl++] = '='; i++; }
						 restr[rl++] = MATH_DELIMCHAR;
						 break;
				case '=':
						 if(strbk[i+1] != '=')
						 {
							 eflag = 1;
							 break;
						 }
						 restr[rl++] = MATH_DELIMCHAR;
						 restr[rl++] = strbk[i++]; 
						 restr[rl++] = strbk[i]; 
						 restr[rl++] = MATH_DELIMCHAR;
						 break;

				case MATH_DELIMCHAR:
						 restr[rl++] = MATH_DELIMCHAR;
						 restr[rl++] = ','; 
						 restr[rl++] = MATH_DELIMCHAR;
						 break;
				case ',':
				default:eflag = 1;break;
			}

			if(eflag == 1)
			{
				break;
			}
#ifdef __MF_DEBUG_MODE_
			printf("restr =  |%s|\n", restr);
#endif
		}
		else
		{
			// bug
			fprintf(stderr, "[analysis]flag = %d", flag);
			break;
		}

	}

	if(pflag > 0)
	{
		fprintf(stderr, "[analysis]parser error:( not paired\n");
	}

	if(eflag == 1)
	{
		fprintf(stderr, "[analysis]parser failed in |%s|\n", strbk + i);
		free(restr);
		restr = NULL;
	}

	free(strbk);
	return restr;
}

char *analysis_v1_0_0(char *str, int strsize)
{
	char *restr = malloc(strsize * 3);
	//strcpy(restr, str);
	// 保留起始空格 测试
	snprintf(restr, strsize * 3, " %s", str);
	stringRemoveSpace(restr + 1);

	int index = 0, olnum = 0;
	// type 1
	for(index =0 ; index < OL_MAX_SIZE; index++)
	{
		if(ol[index].string == NULL)
		{
			break;
		}

		if(ol[index].type == MATH_TYPE_B)
		{
			// skip
			continue;
		}

		olnum++;
		int len = strlen(ol[index].string);
		char *temp = malloc(len + 3);
		char *temp2 = malloc(len + 3);
		snprintf(temp, len + 3, " %s ", ol[index].string);

		if(len >= 2 )
		{
			if(strcmp(ol[index].string, ">=") == 0)
			{
				snprintf(temp2, len + 3, " > =");
			}
			else if(strcmp(ol[index].string, "<=") == 0)
			{
				snprintf(temp2, len+3, " < =");
			}
			else
			{
				snprintf(temp2, len+3, "%s", ol[index].string);
			}

		}
		else
		{
			snprintf(temp2, len+3, "%s", ol[index].string);
		}

		replace(restr, strsize * 3, temp2, temp);

		free(temp);
		free(temp2);
	}

#ifdef __MF_DEBUG_MODE_
	_WriteLog("TYPE_A:restr=|%s|\n", restr);
#endif

	for(index = 0; index < OL_MAX_SIZE; index++)
	{
		if(ol[index].string == NULL)
		{
			break;
		}

		if(ol[index].type != MATH_TYPE_B)
		{
			// skip
			continue;
		}

		olnum++;
		int len = strlen(ol[index].string);
		char *temp = malloc(len + 5);
		snprintf(temp, len + 5, " %s (", ol[index].string);

		char *temp2 = malloc(len + 7);
		//snprintf(temp2, len + 7, " %c%s%c%c(%c", MATH_DELIMCHAR, ol[index].string, MATH_DELIMCHAR, MATH_DELIMCHAR, MATH_DELIMCHAR);
		snprintf(temp2, len + 7, "  %s  (", ol[index].string);
		
		replace(restr, strsize * 3, temp, temp2);

	}

#ifdef __MF_DEBUG_MODE_
	_WriteLog("TYPE_B:restr=|%s|\n", restr);
#endif
	replace(restr, strsize * 3, ")", " ) ");

#ifdef __MF_DEBUG_MODE_
	_WriteLog("):restr=|%s|\n", restr);
#endif
	// deal ()
	int rc = analysis_parenthesis(restr, 1, strsize * 3);

#ifdef __MF_DEBUG_MODE_
	_WriteLog("():restr=|%s|\n", restr);
#endif
	// clear space
	stringRemoveSpace(restr);
#ifdef __MF_DEBUG_MODE_
	_WriteLog("space:restr=|%s|\n", restr);
#endif
	if(rc < 0)
	{
		restr = NULL;
		free(restr);
	}
	return restr;
}

/* ----------------------------------------------------------------------------*/
/**
 * @brief 对栈中的内容进行一次计算
 *
 * @param pchar
 * @param fdata
 * @param level
 *
 * @return 
 */
/* ----------------------------------------------------------------------------*/
int mfCalcStack(operatorchar *pchar, formuladata *fdata, int level)
{
	// debug
#ifdef __MF_DEBUG_MODE_
	if(1)
	{
		printf("mfCalcStack: print operatorchar and formuladata\n");
		int i = 0;
		for( i = 0; i <= pchar->top; i++)
		{
			printf("%s ", ol[pchar->opchar[i]].string);
		}

		printf("\n");
		for( i = 0 ; i <= fdata->top ; i++)
		{
			printf("%lf ", fdata->formulaNumber[i]);
		}
		printf("\n");
	}
#endif
	
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
			operatorcharPop(pchar, oindex);
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

/* ----------------------------------------------------------------------------*/
/**
 * @brief 计算识别后的计算公式的计算结果
 *
 * @param str[]
 * @param strsize
 * @param result
 *
 * @return 
 */
/* ----------------------------------------------------------------------------*/
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
		if(str[index] == MATH_DELIMCHAR)
		{
			// operatorchar
			int len = 0;
			char *s = strtagcpy(str + index + 1, MATH_DELIMITER, &len);
			// skip "#s#"
			index += len + 2;
			if(strcmp(s, "(") == 0)
			{
				operatorcharPop(&pchar, 0);
			}
			else if(strcmp(s, ")") == 0)
			{
				// ()
				rc = mfCalcStack(&pchar, &fdata, 0);
				if(rc < 0)
				{
					free(s);
					fprintf(stderr, "mfCalcStack() failed\n");
					goto EXIT_ERR;
				}

				// skip (
				operatorcharPush(&pchar);

			}
			else if(strcmp(s, ",") == 0)
			{
				rc = mfCalcStack(&pchar, &fdata, 0);
				if(rc < 0)
				{
					free(s);
					fprintf(stderr, "mfCalcStack() failed\n");
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
				rc = mfCalcStack(&pchar, &fdata, olevel);
				if(rc < 0)
				{
					free(s);
					fprintf(stderr, "mfCalcStack() failed\n");
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
			char *numstr = strtagcpy(str + index, MATH_DELIMITER, &len);
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
	
	rc = mfCalcStack(&pchar, &fdata, -1);
	if(rc < 0)
	{
		fprintf(stderr, "mfCalcStack() failed\n");
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

/* ----------------------------------------------------------------------------*/
/**
 * @brief 加法实现 +
 *
 * @param num
 * @param data[]
 * @param result
 *
 * @return 
 */
/* ----------------------------------------------------------------------------*/
int mfSum(int num, double data[], double *result)
{
	*result = data[0] + data[1];
#ifdef __MF_DEBUG_MODE_
	_WriteLog("data[1]=%lf data[0]=%lf\n", data[1], data[0]);
#endif
	return 0;
}

/* ----------------------------------------------------------------------------*/
/**
 * @brief 减法实现 -
 *
 * @param num
 * @param data[]
 * @param result
 *
 * @return 
 */
/* ----------------------------------------------------------------------------*/
int mfSub(int num, double data[], double *result)
{
	*result = data[1] - data[0];
#ifdef __MF_DEBUG_MODE_
	_WriteLog("data[1]=%lf data[0]=%lf\n", data[1], data[0]);
#endif
	return 0;
}

/* ----------------------------------------------------------------------------*/
/**
 * @brief 乘法实现 *
 *
 * @param num
 * @param data[]
 * @param result
 *
 * @return 
 */
/* ----------------------------------------------------------------------------*/
int mfMul(int num, double data[], double *result)
{
	*result = data[1] * data[0];
#ifdef __MF_DEBUG_MODE_
	_WriteLog("data[1]=%lf data[0]=%lf\n", data[1], data[0]);
#endif
	return 0;
}

/* ----------------------------------------------------------------------------*/
/**
 * @brief 除法实现 /
 *
 * @param num
 * @param data[]
 * @param result
 *
 * @return 
 */
/* ----------------------------------------------------------------------------*/
int mfDiv(int num, double data[], double *result)
{
	if(data[0] == 0)
	{
		fprintf(stderr, "divide zero\n");
		return -1;
	}

	*result = data[1] / data[0];
#ifdef __MF_DEBUG_MODE_
	_WriteLog("data[1]=%lf data[0]=%lf\n", data[1], data[0]);
#endif
	return 0;
}

int mfGreaterEqual(int num, double data[], double *result)
{
	if(data[1] >= data[0])
	{
		*result = 1;
	}
	else{
		*result = 0;
	}

#ifdef __MF_DEBUG_MODE_
	_WriteLog("data[1]=%lf data[0]=%lf\n", data[1], data[0]);
#endif
	return 0;
}

int mfGreater(int num, double data[], double *result)
{
	if(data[1] > data[0])
	{
		*result = 1;
	}
	else{
		*result = 0;
	}

#ifdef __MF_DEBUG_MODE_
	_WriteLog("data[1]=%lf data[0]=%lf\n", data[1], data[0]);
#endif
	return 0;
}

int mfLessEqual(int num, double data[], double *result)
{
	if(data[1] <= data[0])
	{
		*result = 1;
	}
	else{
		*result = 0;
	}

#ifdef __MF_DEBUG_MODE_
	_WriteLog("data[1]=%lf data[0]=%lf\n", data[1], data[0]);
#endif
	return 0;
}

int mfLess(int num, double data[], double *result)
{
	if(data[1] < data[0])
	{
		*result = 1;
	}
	else{
		*result = 0;
	}

#ifdef __MF_DEBUG_MODE_
	_WriteLog("data[1]=%lf data[0]=%lf\n", data[1], data[0]);
#endif
	return 0;
}

int mfEqual(int num, double data[], double *result)
{
	if(data[1] == data[0])
	{
		*result = 1;
	}
	else{
		*result = 0;
	}

#ifdef __MF_DEBUG_MODE_
	_WriteLog("data[1]=%lf data[0]=%lf\n", data[1], data[0]);
#endif
	return 0;
}

/* ----------------------------------------------------------------------------*/
/**
 * @brief 初始化 添加默认实现的运算符号
 *
 * @return 
 */
/* ----------------------------------------------------------------------------*/
int mathformulaInit()
{
	initflag = 1;

	operatorListAdd("(", LEVEL_ZERO, 0, MATH_TYPE_Z, NULL);
	operatorListAdd(",", LEVEL_ZERO, 0, MATH_TYPE_Z, NULL);
	operatorListAdd("==", LEVEL_ONE, 2, MATH_TYPE_A, mfEqual);
	operatorListAdd(">", LEVEL_ONE, 2, MATH_TYPE_A, mfGreater);
	operatorListAdd(">=", LEVEL_ONE, 2, MATH_TYPE_A, mfGreaterEqual);
	operatorListAdd("<", LEVEL_ONE, 2, MATH_TYPE_A, mfLess);
	operatorListAdd("<=", LEVEL_ONE, 2, MATH_TYPE_A, mfLessEqual);

	operatorListAdd("+", LEVEL_TWO, 2, MATH_TYPE_A, mfSum);
	operatorListAdd("-", LEVEL_TWO, 2, MATH_TYPE_A, mfSub);
	operatorListAdd("*", LEVEL_THREE, 2, MATH_TYPE_A, mfMul);
	operatorListAdd("/", LEVEL_THREE, 2, MATH_TYPE_A, mfDiv);

	return 0;
}
/// check function 
/* ----------------------------------------------------------------------------*/
/**
 * @brief 计算计算公式的结果 外部调用函数
 *
 * @param str
 * @param result
 *
 * @return 
 */
/* ----------------------------------------------------------------------------*/
int mathformula(char *str, double *result)
{
	if(initflag == 0)
	{
		mathformulaInit();
	}

	if(str == NULL)
	{
		fprintf(stderr, "empty parameter\n");
		return -1;
	}

	char *p = analysis(str, strlen(str));
	// debug info
#ifdef __MF_DEBUG_MODE_
	_WriteLog("debug:p = |%s|\n", p);
#endif
	if(p == NULL) return -1;
	int rc = mfTransformation(p, strlen(p), result);
	free(p);

	return rc;
}
