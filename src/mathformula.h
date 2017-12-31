#ifndef __MATHFORMULA_H_
#define __MATHFORMULA_H_

#define OL_MAX_SIZE 255
#define MATH_DELIMITER "#"
#define MATH_DELIMCHAR '#'

#define MATH_TYPE_A 1      // type a (operator) b  example +-*/ 
#define MATH_TYPE_B 2      // type operator(a,b)   example sum(a,b)
#define MATH_TYPE_Z 0      // (

typedef enum{
	LEVEL_ZERO,
	LEVEL_ONE,
	LEVEL_TWO,
	LEVEL_THREE,
	LEVEL_FOUR,
	LEVEL_FIVE,
	LEVEL_SIX,
	LEVEL_SEVEN,
	LEVEL_EIGHT,
	LEVEL_NINE,
	LEVEL_TEN
}MATH_LEVEL;

typedef int mfDescToValue_func(char *desc, double *data);
typedef int mfCalc_func(int num, double data[], double *result);

typedef struct operatorList{
	int id;
	char *string;
	int level;
	int number;
	int type;       // type 1 +-*/ a string b   // type 2 log(a,b)...
	int (*mfCalc_func)(int num, double data[], double *result);
}operatorList;

/* ----------------------------------------------------------------------------*/
/**
 * @brief 计算公式中的运算符号栈
 */
/* ----------------------------------------------------------------------------*/
typedef struct operatorchar{
	char *opchar;     ///计算公式中的运算符号栈空间  opchar == ol->id
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

// function
int operatorListAdd(char *string, int level, int number, int type, int (*mfCalc_func)(int num,double data[], double *result));
int mathformula(char *str, double *result);

#endif
