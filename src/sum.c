#include<stdio.h>
#include<stdlib.h>
#include<math.h>
#define maxsize 50
void trans(char str[],char exp[])/*将算术表达式str转换成后缀表达式exp*/
{
	struct
	{ char data[maxsize]; /*存放运算符*/
		int top;            /*栈指针*/
	}opr;                 /*定义运算符栈*/
	char ch;
	int i=0,t=0;         /*t作为exp的下标，i作为str的下标*/
	opr.top=-1;          /*初始化设定top的值为负一*/
	ch=str[i];i++;       /*逐个读取字符串中的字符*/
	while (ch!='\0')     /*str表达式未扫描完时循环*/
	{  switch(ch)        /*判定*/
		{
			case '(':
				opr.top++;opr.data[opr.top]=ch; /*判定为'('号，则将其入栈opr*/
				break;
			case ')':
				while (opr.data[opr.top]!='(')  /*判定为')'号*/
				{ exp[t]=opr.data[opr.top];    /*将栈opr中'('以后的字符依次删除并存入数组exp中*/
					opr.top--;
					t++;
				}
				opr.top--;          /*将左括号删除*/
				break;
			case  '+':           /*判定为加号或减号*/
			case  '-':
				while (opr.top!=-1 &&opr.data[opr.top]!='(')
				{      exp[t]=opr.data[opr.top];    /*将当前栈opr中(以前的所有字符依次删除并存入数组exp中*/
					opr.top--;
					t++;
				}
				opr.top++;opr.data[opr.top]=ch;  /*将ch存入栈opr中*/
				break;
			case '*':
			case '/':
				while (opr.data[opr.top]=='*'||opr.data[opr.top]=='/'||opr.data[opr.top]=='^')
				{ exp[t]=opr.data[opr.top];   /*将当前栈opr中连续的'*'或'/'或'^'依次删除并存入数组exp中*/
					opr.top--;
					t++;
				}
				opr.top++;opr.data[opr.top]=ch;  /*将ch存入栈opr中*/
				break;
			case '^':                   /*判定为乘方号*/
				while (opr.data[opr.top]=='^')
				{ exp[t]=opr.data[opr.top];   /*将当前栈opr中连续的'^'依次删除并存入数组exp中*/
					opr.top--;
					t++;
				}
				opr.top++;opr.data[opr.top]=ch;  /*将ch存入栈opr中*/
				break;
			case ' ': break;              /*过滤掉空格*/
			default:
					  while(ch>='0'&& ch<='9'||ch=='.')  /*判定为数字*/
					  { exp[t]=ch;t++;          /*将后续数字依次存入数组中*/
						  ch=str[i];i++;
					  }
					  i--;
					  exp[t]='#';t++;          /*用#标示一个数值串结束*/
		}
		ch=str[i];i++;
	}
	while (opr.top!=-1)         /*此时str扫描完毕，栈不空时循环*/
	{  exp[t]=opr.data[opr.top];
		t++;opr.top--;
	}
	exp[t]='\0'; /*给exp表达式添加结束标示*/
}
float compvalue(char exp[])    /*计算后缀表达式的值*/
{
	struct
	{   float data[maxsize];  /*存放数值*/
		int top;             /*栈指针*/
	} st;                    /*定义数值栈*/
	float d,d2;double po;
	char  ch;
	int t=0,flag=1,i,count;                     /*t作为exp的下标*/
	st.top=-1;
	ch=exp[t];t++;
	while (ch!='\0')          /*exp字符串为扫描完时循环*/
	{   switch(ch)
		{
			case '+':st.data[st.top-1]=st.data[st.top-1]+st.data[st.top]; /*执行两次退栈，并将计算结果入栈*/
					 st.top--;break;
			case '-':st.data[st.top-1]=st.data[st.top-1]-st.data[st.top];
					 st.top--;break;
			case '*':st.data[st.top-1]=st.data[st.top-1]*st.data[st.top];
					 st.top--;break;
			case '/':
					 if(st.data[st.top]!=0)
						 st.data[st.top-1]=st.data[st.top-1]/st.data[st.top];
					 else
					 {    printf("\n除零错误！\n");
						 exit(0);                    /*除数为零，异常退出*/
					 }
					 st.top--;break;
			case '^':
					 po=pow(st.data[st.top-1],st.data[st.top]); st.data[st.top-1]=(float)po;/*调用pow子函数进行乘方运算*/
					 st.top--;break;
			default:
					 d=0;  flag=1; d2=0;                           /*将数字字符转换成对应的数值存放到d中*/
					 while(ch>='0'&&ch<='9'&&flag)          /*判定为数字字符*/
					 {  d=10*d+ch-'0';
						 ch=exp[t];t++;
						 if(ch=='.')
							 flag=0;

					 }
					 if(flag==0)
					 {  ch=exp[t];t++;count=0;
						 while(ch>='0'&&ch<='9')          /*判定为数字字符*/
						 {d2=10*d2+ch-'0';
							 ch=exp[t];t++;count++;
						 }
						 for(i=1;i<=count;i++)
							 d2=0.1*d2;
					 }
					 d+=d2;
					 st.top++;
					 st.data[st.top]=d;
		}
		ch=exp[t];t++;
	}
	return st.data[st.top];
}
int main()
{
	char str[maxsize],exp[maxsize];  /*str存储原算术表达式，exp存储对应的后缀表达式*/
	printf("the arithmetic expression is:\n");
	gets(str);
	trans(str,exp);
	printf("the postfix expression is:%s\n",exp);
	printf("the result is %g\n",compvalue(exp));
}
