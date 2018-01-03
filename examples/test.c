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

int lessEqual(int num, double data[], double *result)
{
	if(data[1] <= data[0])
	{
		*result = 1;
	}
	else{
		*result = 0;
	}

	return 0;
}

int mfIf(int num , double data[], double *result)
{
	if(data[0] == 0)
	{
		*result = 0;
	}
	else
	{
		*result = 1;
	}

	return 0;
}

int main()
{
	//operatorListAdd(">=", LEVEL_TEN, 2, MATH_TYPE_A, greatEqual);
	//operatorListAdd("<=", LEVEL_TEN, 2, MATH_TYPE_A, lessEqual);
	int rc = operatorListAdd("IF", LEVEL_TEN, 1, MATH_TYPE_B, mfIf);
	//printf("rc = %d\n", rc);
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
