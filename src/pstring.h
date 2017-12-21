#ifndef __PSTRING_H_
#define __PSTRING_H_

typedef struct ptype{
	int flag; // 0 normal 1 in use 2 not use
	char *type;
	int length;
	char *end; // end tag
	struct ptype *next;
}ptype;

int strcat_py(char **str, int *size, int num, char *fmt, ...)
	__attribute__((format(printf, 4,5)));
int snprintf_p(char **str, int *size, int num, char *fmt, ...)
	__attribute__((format(printf, 4, 5)));
char *strcpy_p(char *str);
char *strncpy_p(char *str, int len);
char *strtagcpy(char *str, char *tag, int *len);

int utf8_strlen(const char * s);
char * inttostring(int num);

int stringRemoveChar(char *str, char ch);
int stringRemoveSpace(char *str);
void cstring_trim(char *str);
void replace(char originalString[], int len, char key[], char swap[]);
int stringEndOfStr(char *string, char *end);

int charToNumber(char ch);
int parseNumber(char *number, int *value);
int checkPType(char *str, ptype *head);
int strFormat(char *str, char *fmt, ...)
	__attribute__((format(printf, 2,3)));

int ptype_free(ptype *p);

#endif
