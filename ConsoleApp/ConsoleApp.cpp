#include "stdafx.h"
#include "CommonCPP\Common.h"
//#include <LIEF\LIEF.hpp>

int i = 0;
LONG WINAPI callback (struct _EXCEPTION_POINTERS *ExceptionInfo) {
	i = 2;
	return EXCEPTION_CONTINUE_EXECUTION;
}


int main(int argc, char *argv[])
{
	SetUnhandledExceptionFilter(callback);
	int ret = 10 / i;
	printf("程序正常执行：ret = %d\n", ret);
	getchar();
	return 0;
}

