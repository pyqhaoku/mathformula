/**
 * @file pstring.c
 * @brief 字符串处理函数
 * @author pyq mail:pyqhaoku@163.com
 * @version v1.0
 * @date 2017-06-30
 *
 * ===========================================================
 * @date 2017-06-30
 * @version v1.0
 * 字符串处理函数 当长度不够时申请内存
 * -----------------------------------------------------------
 *  @date 2017-07-26
 *  @version v1.0
 *
 * realloc后释放内存后导致异常退出
 * ----------------------------------------------------------------------------
 *  @date 2017-08-11
 *  @version v2.0
 *  @author pyq(pyqhaoku@163.com)
 *
 *  添加strncpy_p strcpy_p parseNumber checkPType strFormat
 * ===========================================================
 */

#ifndef _GNU_SOURCE
#define _GNU_SOURCE
#endif

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <stdarg.h>
#include <ctype.h>
#include <math.h>
#include <float.h>
#include <limits.h>

#include "pstring.h"

/* --------------------------------------------------------------------------*/
/**
 * @brief 追加字符串函数
 *
 * @note 自定义函数 当长度不够时自动申请内存
 *
 * @param str
 * @param size
 * @param num
 * @param fmt
 * @param ...
 *
 * @return 
 *
 * ============================================================================
 * @date 2017-07-26
 * @version v2.0
 * @author pyq
 *
 * 修复当*str为NULL时程序异常退出的bug
 * 修复没有在最后添加\0的bug
 * ============================================================================
 */
/* ----------------------------------------------------------------------------*/
int strcat_py(char **str, int *size, int num, char *fmt, ...)
{
	va_list arg;
	int len = *str ? strlen(*str) : 0;
	int totallen = len;
	va_start(arg, fmt);
	/// total len
	totallen += vsnprintf(NULL, 0, fmt, arg); 
	if (totallen > (* size))
	{
		*size = totallen * num + 1;
		char *bak = realloc((*str), *size );
		if(bak == NULL)
		{
			va_end(arg);
			return -1;
		}
		else
		{
			//if(*str )
			//{
				//free(*str);
			//}
			*str = bak;
		}
	}

	va_start(arg, fmt);
	vsnprintf(*str + len, *size - len, fmt, arg);
	va_end(arg);
	(*str)[totallen] = 0;
	return 0;
}

/* ----------------------------------------------------------------------------*/
/**
 * @brief snprintf修改版本 自动申请内存
 *
 * @param str
 * @param size
 * @param num
 * @param fmt
 * @param ...
 *
 * @return 
 */
/* ----------------------------------------------------------------------------*/
int snprintf_p(char **str, int *size, int num, char *fmt, ...)
{
	va_list arg;
	va_start(arg, fmt);
	int totallen = vsnprintf(NULL, 0, fmt, arg);
	if(totallen >= (*size))
	{
		*size = totallen * num + 1;
		char *bak = realloc((*str), *size );
		if(bak == NULL)
		{
			va_end(arg);
			return -1;
		}
		else
		{
			//if(*str )
			//{
				//free(*str);
			//}
			*str = bak;
		}
	}

	va_start(arg, fmt);
	vsnprintf(*str, *size, fmt, arg);
	va_end(arg);
	(*str)[totallen] = 0;
	return 0;
}

/* ----------------------------------------------------------------------------*/
/**
 * @brief 复制字符串
 *
 * @note 使用后需要释放
 *
 * @param str
 *
 * @return 
 */
/* ----------------------------------------------------------------------------*/
char *strcpy_p(char *str)
{
	int len = strlen(str);
	char *save = malloc(len + 1);

	memset(save, 0, len+1);
	strncpy(save, str, len);
	save[len] = 0;
	return save;
}

/* ----------------------------------------------------------------------------*/
/**
 * @brief 复制字符串的前N个字符
 *
 * @param str
 * @param len
 *
 * @return 
 */
/* ----------------------------------------------------------------------------*/
char *strncpy_p(char *str, int len)
{
	int len2 = strlen(str);
	if(len2 > len) len2 = len;

	char *save = malloc(len2 + 1);
	memset(save, 0, len2 + 1);
	strncpy(save, str, len2);
	save[len2] = 0;
	return save;
}

/* ----------------------------------------------------------------------------*/
/**
 * @brief 复制字符串 直到遇到结束符或者 tag
 *
 * @param str
 * @param tag
 *
 * @return 
 */
/* ----------------------------------------------------------------------------*/
char *strtagcpy(char *str, char *tag, int *len)
{
	char *p = strcasestr(str, tag);
	int l = p ? p - str : strlen(str);
	if(len != NULL)
	{
		*len = l;
	}

	return strncpy_p(str, l);
}

/* ----------------------------------------------------------------------------*/
/**
 * @brief 移除字符串的某个字符
 *
 * @param str
 * @param ch
 *
 * @return 
 */
/* ----------------------------------------------------------------------------*/
int stringRemoveChar(char *str, char ch)
{
	if(str == NULL)
	{
		// errorlog
		return -1;
	}

	int i = 0, l = strlen(str);
	int index = 0;
	for( i = 0; i < l ; i++)
	{
		if(str[i] != ch)
		{
			if(index -i)
			{
				str[index++] = str[i];
			}
			else
			{
				index++;
			}
		}
	}

	str[index] = 0;

	return 0;
}

int stringRemoveSpace(char *str)
{
	if(str == NULL)
	{
		// errorlog
		return -1;
	}

	int i = 0, l = strlen(str);
	int index = 0;
	for( i = 0; i < l ; i++)
	{
		//if(str[i] != ' ')
		if(isblank(str[i]) == 0)
		{
			if(index -i)
			{
				str[index++] = str[i];
			}
			else
			{
				index++;
			}
		}
	}

	str[index] = 0;

	return 0;
}

//remove  left/right space
void cstring_trim(char *str)
{
	if(str == NULL) return;
	int index = 0,i = 0,j = 0;
	index = strlen(str);

	i = index - 1;
	while(str[i] == ' ' || str[i] == '\r' || str[i] == '\t' || str[i] == '\n')i--;
	str[i+1] = 0;

	for (i = 0; i < index; i++)
	{
		if(str[i] != ' ' && str[i] != '\r' && str[i] != '\t' && str[i] != '\n')
		{
			if(i == 0) break;
			for(j = 0 ; j < index - i ; j++)
			{
				str[j] = str[j+ i];
			}
			str[j] = 0;
			break;
		}
	}

}

/* ----------------------------------------------------------------------------*/
/**
 * @brief 字符串替换函数
 *
 * @param originalString[]
 * @param len
 * @param key[]
 * @param swap[]
 */
/* ----------------------------------------------------------------------------*/
void replace(char originalString[], int len, char key[], char swap[])
{
	int lengthOfOriginalString = 0, lengthOfKey = 0, lengthOfSwap = 0, i = 0, j = 0, flag = 0;
	char tmp[4096];


	lengthOfOriginalString = strlen(originalString);
	lengthOfKey = strlen(key);
	lengthOfSwap = strlen(swap);

	for( i = 0; i <= lengthOfOriginalString - lengthOfKey; i++)
	{
		flag = 1;

		for(j  = 0; j < lengthOfKey; j ++){
			if(originalString[i + j] != key[j]){
				flag = 0;
				break;
			}
		}

		if(flag && lengthOfOriginalString + lengthOfSwap - lengthOfKey < len){
			strcpy(tmp, originalString);
			strcpy(&tmp[i], swap);
			strcpy(&tmp[i + lengthOfSwap], &originalString[i  + lengthOfKey]);
			strcpy(originalString, tmp);
			i += lengthOfSwap - 1;
			lengthOfOriginalString = strlen(originalString);
		}
	}
}

/* ----------------------------------------------------------------------------*/
/**
 * @brief 判断字符串是否以某个字符串结尾 是返回1 不是返回0
 *
 * @param string
 * @param end
 *
 * @return 
 */
/* ----------------------------------------------------------------------------*/
int stringEndOfStr(char *string, char *end)
{
	if(string == NULL || end == NULL)
	{
		return -1;
	}

	int le = strlen(end);
	int ls = strlen(string);

	if(le > ls)
	{
		return 0;
	}

	int rc = 1;
	int i = 1;
	for( i = 1; i <= le; i++)
	{
		if(string[ls-i] != end[le-i])
		{
			rc = 0;
			break;
		}
	}

	return rc;
}

/* --------------------------------------------------------------------------*/
/**
 * @brief 将字符类型转换成数字 0-9转换为数字0-9 ABCDEF转换为数字10-15
 *
 * @param ch
 *
 * @return 
 */
/* ----------------------------------------------------------------------------*/
int charToNumber(char ch)
{
	switch(ch)
	{
		case '0':return 0;
		case '1':return 1;
		case '2':return 2;
		case '3':return 3;
		case '4':return 4;
		case '5':return 5;
		case '6':return 6;
		case '7':return 7;
		case '8':return 8;
		case '9':return 9;
		case 'a':
		case 'A':return 10;
		case 'b':
		case 'B':return 11;
		case 'c':
		case 'C':return 12;
		case 'd':
		case 'D':return 13;
		case 'e':
		case 'E':return 14;
		case 'f':
		case 'F':return 15;
		default:return -1;
	}

	return -1;
}

/* --------------------------------------------------------------------------*/
/**
 * @brief 将字符串转化为数字类型 返回转化的字符串长度
 *
 * @param number
 * @param value
 *
 * @return 
 */
/* ----------------------------------------------------------------------------*/
int parseNumber(char *number, int *value)
{
	if(number == NULL)
	{
		return -1;
	}

	int ln = strlen(number);
	if(ln == 0)
	{
		return -1;
	}
	
	int sign = 1;
	// 16 flag
	int hex = 10;
	*value = 0;
	int index = 0;
	if(number[0] == '+')
	{
		index = 1;
	}
	else if(number[0] == '-')
	{
		sign = -1;
		index = 1;
	}
	else if(number[0] == '0' && (number[1] == 'x' || number[1] == 'X'))
	{
		hex = 16;
		index += 2;
	}
	for(; index < ln; index++)
	{
		int v1 = charToNumber(number[index]);
		if(v1 < 0 || v1 > hex)
		{
			*value *= sign;
			return index;
		}
		//if(isdigit(number[index]) == 0)
		//{
			//*value *= sign;
			//return index;
		//}
		*value = (*value) * hex + v1;
	}

	*value *= sign;
	return index;
}

/* ----------------------------------------------------------------------------*/
/**
 * @brief 读取字符串中括号内的内容
 * @note example fa(b(cd))  return b(cd)
 * 如果括号数量不匹配 返回NULL
 * 返回的字符串需要手动释放
 *
 * @param string
 *
 * @return 
 */
/* ----------------------------------------------------------------------------*/
char *parseParenthesis(char *string)
{
	int len = strlen(string);
	int sindex = -1, eindex = -1, eflag = 0, i = 0;
	for( i = 0; i < len; i++)
	{
		if(string[i] == ')')
		{
			eflag--;
			if(eflag <= 0)
			{
				eindex = i;
				break;
			}
		}
		else if(string[i] == '(')
		{
			eflag++;
			if(sindex < 0)
			{
				sindex = i + 1;
			}
		}
	}

	if(eindex >= 0 && sindex >= 0 && eflag == 0)
	{
		return strncpy_p(string + sindex, eindex - sindex);
	}

	return NULL;
}

/* ----------------------------------------------------------------------------*/
/**
 * @brief 检查匹配类型
 *
 * @note %p.%3p-%-4p.%4p.xml  
 * str=abc.def-0801.xml or abc.def-0801-1000.xml  right return 0
 * str=abc.def-080110.xml wrong return -1
 * 判断ptype和字符串是否匹配 不匹配返回-1 匹配返回0 未确定返回1
 * 例: type=%p.%3p-%-4p.%4p.xml   其中%-4p. 表示4长度的字符串 -表示字符串不一定有
 * 当str=abc.def-0801.xml 时 %-4p.%4p.xml %-4p. 被省略了 虽然符合格式 但是需要把 %-4p去掉
 * 所以运行结果为 返回1 然后修改 %-4p. 的标志  修改标志的原则是 匹配失败时 最后一个标志
 * 当所有标志都修改后 还是匹配不上时 返回-1
 *
 * @param str
 * @param head
 *
 * @return 
 */
/* ----------------------------------------------------------------------------*/
int checkPType(char *str, ptype *head)
{
	ptype *cur = head;
	ptype *pf = NULL;
	char *ptr = str;

	while(cur )
	{
		if(cur->flag == 1)
		{
			pf = cur;
		}
		else if(cur->flag == 2)
		{
			cur = cur->next;
			continue;
		}

		if(cur->length > 0)
		{
			ptr += cur->length;
			// cmpare tail
			int rc = strncmp(ptr, cur->end, strlen(cur->end));
			if(rc == 0)
			{
				// right
				ptr += strlen(cur->end);
				cur = cur->next;
				continue;
			}
			else
			{
				// failed
				if(pf == NULL)
				{
					return -1;
				}
				else
				{
					pf->flag = 2;
					return 1;
				}
			}
		}
		else
		{
			char *p = strstr(ptr, cur->end);
			if(p == NULL)
			{
				// failed
				if(pf == NULL)
				{
					return -1;
				}
				else
				{
					pf->flag = 2;
					return 1;
				}
			}
			else
			{
				// set position
				ptr = p + strlen(cur->end);
				cur = cur->next;
				continue;
			}
		}
	}

	return 0;
}

/* ----------------------------------------------------------------------------*/
/**
 * @brief 将字符串按fmt格式分隔
 * @note %[-][0-9]p[.*] fmt格式 -表示可以省略 数字表示长度 p后面表示连接字符串 fmt可以由多个 组成  p是用来编译检查的 
 *
 * @param str
 * @param fmt
 * @param ...
 *
 * @return 
 */
/* ----------------------------------------------------------------------------*/
int strFormat(char *str, char *fmt, ...)
{
	va_list arg;

	// check fmt
	char *ptr, *save = NULL;
	if(fmt == NULL || strlen(fmt) <= 2)
	{
		// error
		return -1;
	}

	ptype *head = NULL, *cur = NULL;
	char *fmtb = strncpy_p(fmt, strlen(fmt));
	for(ptr = strtok_r(fmtb, "%", &save); ptr != NULL; ptr = strtok_r(NULL, "%", &save))
	{
		if(strlen(ptr) == 0)
		{
			continue;
		}

		// - number p

		ptype *nt = malloc(sizeof(ptype));
		memset(nt, 0, sizeof(ptype));
		if(ptr[0] == '-')
		{
			nt->flag = 1;
			// skip -
			ptr++;
		}
		else 
		{
			nt->flag = 0;
		}

		// deal number
		if(isdigit(ptr[0]) )
		{
			// number
			int len = parseNumber(ptr, &nt->length);
			ptr += len;
		}

		// deal type
		if(ptr[0] == 'p')
		{
			ptr++;
		}

		// get tail
		nt->end = strncpy_p(ptr, strlen(ptr));

		// add to ptype linklist
		if(cur == NULL)
		{
			head = nt;
			cur = head;
		}
		else
		{
			cur->next = nt;
			cur = nt;
		}
	}

	free(fmtb);

	int rc = 0;
	do{
		rc = checkPType(str, head);
	}while(rc == 1);

	if(rc == -1)
	{
		// ptype error
		ptype_free(head);
		return -1;
	}

	// check ptype
	va_start(arg, fmt);
	char **p = NULL;
	char *buf = str;
	for(cur = head; cur != NULL; cur = cur->next)
	{
		p = va_arg(arg, char **);
		if(cur->flag == 2)
		{
			continue;
		}
		else
		{
			int len = 0;
			if(cur->length > 0)
			{
				len = cur->length;
			}
			else if(strlen(cur->end) > 0)
			{
				char *ptr = strstr(buf, cur->end);
				if(ptr == NULL)
				{
					// bug 
					va_end(arg);
					ptype_free(head);
					return -1;
				}

				len = ptr - buf;
			}
			else if(cur->next == NULL)
			{
				// last %p read all
				len = strlen(buf);
			}
			if(p != NULL)
			{
				*p = strncpy_p(buf, len);
			}
			buf += len + strlen(cur->end);
		}
	}

	va_end(arg);
	ptype_free(head);

	return 0;
}

/* ----------------------------------------------------------------------------*/
/**
 * @brief 释放申请的资源
 *
 * @param p
 *
 * @return 
 */
/* ----------------------------------------------------------------------------*/
int ptype_free(ptype *p)
{
	ptype *cur = p, *cn = p;
	if(p == NULL)
	{
		return 0;
	}

	do{
		if(cur->type)
			free(cur->type);
		if(cur->end)
			free(cur->end);

		cn = cur->next;
		free(cur);
		cur = cn;
	}while(cn);

	return 0;
}

/* ----------------------------------------------------------------------------*/
/**
 * @brief utf8字符串长度计算 能计算中英文混合的情况
 *
 * @param s
 *
 * @return 
 */
/* ----------------------------------------------------------------------------*/
int utf8_strlen(const char * s)
{
	int i, j;
	int len = 0;
	for(i = 0; s[i] != 0; i++){
		if(s[i] & 0x80){
			j = 1;
			while((s[++i] & 0x80) && !(s[i] & 0x40)) j++;
			len += 2;
			i--;
		}
		else{
			len++;
		}
	}
	return len;
}

/* ----------------------------------------------------------------------------*/
/**
 * @brief 计算字符串被空格分割的子串数量
 * @note  \t\n等符号也算作子串  只有空格的情况返回0
 *
 * @param s
 *
 * @return 
 */
/* ----------------------------------------------------------------------------*/
int stringNumber(const char *s)
{
	int num = 0, len = strlen(s);
	int i = 0;
	while(i < len && s[i] == ' ')i++;
	if(i == len) 
		return 0;

	for( ; i < len; i++)
	{
		if(s[i] == ' ')
		{
			while(s[i+1] == ' ')i++;
		}
		else
		{
			num++;
			while(s[i+1] != ' ') i++;
		}
	}

	return num;
}

/* ----------------------------------------------------------------------------*/
/**
 * @brief 将数字转换成字符串
 *
 * @param num
 *
 * @return 
 */
/* ----------------------------------------------------------------------------*/
char * inttostring(int num)
{
	static char numstr[20];
	snprintf(numstr, sizeof(numstr), "%d", num);
	return numstr;
}

char *doubletostring(double num)
{
	static char numstr[64];
	double dnum = floor(num);
	if(fabs(num - dnum) <= DBL_EPSILON && num <= INT_MAX && num >= INT_MIN)
	{
		snprintf(numstr, sizeof(numstr), "%d", (int)dnum);
	}
	else
	{
		if(fabs(num - dnum) <= DBL_EPSILON && fabs(num) < 1.0e60)
		{
			snprintf(numstr, sizeof(numstr), "%.0f", num);
		}
		else if(fabs(num) < 1.0e-6 || fabs(num) > 1.0e9)
		{
			snprintf(numstr, sizeof(numstr), "%e", num);
		}
		else
		{
			snprintf(numstr, sizeof(numstr), "%f", num);
		}
	}
	return numstr;
}

//#define __TEST_PSTRING_
#ifdef __TEST_PSTRING_

int main()
{
	//test

	double a = 0;
	while(~scanf("%lf", &a))
	{
		printf("a=%f,|%s|\n", a, doubletostring(a));
	}

	return 0;
}

#endif
