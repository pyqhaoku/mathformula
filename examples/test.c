#include <stdio.h>

#include <mathformula.h>

int testf(char *desc, double *re)
{
	if(desc == NULL)
	{
		*re = 0;
	}
	else if(strcmp(desc, "PI") == 0)
	{
		*re = 3.1415;
	}
	else if(strcmp(desc, "true") == 0)
	{
		*re = 1;
	}
	else if(strcmp(desc, "eee") == 0)
	{
		*re = 2.7;
	}

	return 0;
}

int main()
{
	mfDescToValue = testf;
	char test[256];
	double result;
	while(~scanf("%s", test))
	{
		int rc = mathformula(test, &result);
		if(rc == 0)
		{
			printf("result=%lf\n", result);
		}
		else
		{
			printf("mathformula(%s) failed\n", test);
		}
	}

	return 0;
}
