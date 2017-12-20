#ifndef __MATHFORMULA_H_
#define __MATHFORMULA_H_

/* ----------------------------------------------------------------------------*/
/**
 * @brief 计算公式中的运算符号栈
 */
/* ----------------------------------------------------------------------------*/
typedef struct operatorchar{
	char *opchar;     ///计算公式中的运算符号栈空间
	int ocsize;       /// 栈申请的空间
	int top;         /// 栈指针
}operatorchar;

/* ----------------------------------------------------------------------------*/
/**
 * @brief 计算公式中的数字
 */
/* ----------------------------------------------------------------------------*/
typedef struct formuladata{
	char **formulaString;         /// 计算公式中的字符串列表
	double *formulaNumber;          /// 计算公式字符串转换后的数字
	int fdsize;                   /// 申请的长度大小
	int top;        /// 栈指针 default -1 stack
}formuladata;

//typedef struct 

extern mfDescToValue_func *mfDescToValue;

#endif
