/*
	CommandPrmpt.cpp
	프로그램 설명: 명령 프롬프트의 골격
*/

#include <stdio.h>
#include <stdlib.h>
#include <tchar.h>
#include <locale.h>
#include <Windows.h>

#define STR_LEN 256
#define CMD_TOKEN_NUM 10

TCHAR ERROR_CMD[] = _T("'%s'은(는) 실행할 수 있는 프로그램이 아닙니다. \n");

int CmdProcessing(int);
TCHAR * StrLower(TCHAR *);
int CmdReadTokenize(void);
void CmdInput(void);


TCHAR cmdString[STR_LEN];
TCHAR cmdTokenList[CMD_TOKEN_NUM][STR_LEN];
TCHAR seps[] = _T(" ,\t\n");

int _tmain(int argc, TCHAR * argv[])
{
	// 한글 입력을 가능케 하기 위해
	_tsetlocale(LC_ALL, _T("Korean"));

	/*
	int i;
	_tprintf("argc: %d\n", argc);
	for (i = 0; i < argc; i++)
		_tprintf("argv[%d]: %s\n", i, argv[i]);
	*/

	// start command
	if (argc > 1)
	{
		int i;
		for (i = 1; i < argc; i++)
			_tcscpy(cmdTokenList[i - 1], argv[i]);
		CmdProcessing(argc - 1); // start 이후 (미포함) 명령어 처리
	}

	DWORD isExit;
	while (1)
	{
		CmdInput();
		int tokenNum = CmdReadTokenize();
		isExit = CmdProcessing(tokenNum);
		{
			if (isExit == TRUE)
			{
				_fputts(_T("명령어 처리를 종료합니다. \n"), stdout);
				break;
			}
		}
	}

	return 0;
}
/**********************************************************************
함수: int CmdReadTokenize(void)
기능: CommandPrmpt_Two.cpp의 CmdProcessing 함수는 사용자의 선택을 입력받는 기능과
선택에 따른 명령어 처리 기능을 동시에 지니고 있다. 이에 사용자의 선택을 입력받는 기능을
CmdreadTokenize 함수로 분리시켰다. 명령어가 main 함수를 통해 전달되는 경우에는 사용자 입력이 불필요하기 때문이다.
**********************************************************************/
int CmdReadTokenize(void)
{
	TCHAR * token = _tcstok(cmdString, seps);
	int tokenNum = 0;

	while (token != NULL)
	{
		_tcscpy(cmdTokenList[tokenNum++], StrLower(token));
		token = _tcstok(NULL, seps);
	}

	return tokenNum;
}

void CmdInput(void)
{
	_fputts(_T("Best command prompt>> "), stdout);
	_getts_s(cmdString); // _getts not working
}
/**********************************************************************
함수: THCAR int CmdProcessing(void)   // TCHAR 의미가..?
기능: 명령어를 입력 받아서 해당 명령어에 지정되어 있는 기능을 수행한다.
	  exit가 입력되면 TRUE를 반환하고 이는 프로그램의 종료로 이어진다.
**********************************************************************/
int CmdProcessing(int tokenNum)
{

	if (!_tcscmp(cmdTokenList[0], _T("exit")))
		return TRUE;

	else if (!_tcscmp(cmdTokenList[0], _T("start")))
	{
		TCHAR optString[STR_LEN] = { 0 };
		TCHAR cmdStringWithOptions[STR_LEN] = { 0 };
		if (tokenNum > 1)
		{
			int i;
			for (i = 1; i < tokenNum; i++)
				_stprintf(optString, _T("%s %s"), optString, cmdTokenList[i]);
		}
		_stprintf(cmdStringWithOptions, _T("%s %s"), _T("Project3.exe"), optString); // Project3.exe (start) (command)
		STARTUPINFO si = { 0, };
		PROCESS_INFORMATION pi;
		si.cb = sizeof(si);
		BOOL isRun = CreateProcess(NULL, cmdStringWithOptions, NULL, NULL, TRUE, CREATE_NEW_CONSOLE, NULL, NULL, &si, &pi);
		CloseHandle(pi.hProcess);
		CloseHandle(pi.hThread);
	}

	else if (!_tcscmp(cmdTokenList[0], _T("echo")))
	{
		TCHAR optString[STR_LEN] = { 0 };
		TCHAR cmdStringWithOptions[STR_LEN] = { 0 };
		int i;
		for (i = 1; i < tokenNum; i++)
			_stprintf(optString, _T("%s %s"), optString, cmdTokenList[i]);
		_tprintf(_T("echo message: %s \n"), optString);
	}

	else if (!_tcscmp(cmdTokenList[0], _T("추가 되는 명령어")))
	{

	}

	else
	{
		TCHAR optString[STR_LEN] = { 0 };
		_tcscpy(optString, cmdTokenList[0]);

		int i;
		for (i = 1; i < tokenNum; i++)
			_stprintf(optString, _T("%s %s"), optString, cmdTokenList[i]);

		STARTUPINFO si = { 0, };
		PROCESS_INFORMATION pi;
		si.cb = sizeof(si);
		BOOL isRun = CreateProcess(NULL, optString, NULL, NULL, TRUE, 0, NULL, NULL, &si, &pi);
		CloseHandle(pi.hProcess);
		CloseHandle(pi.hThread);
		if (isRun == FALSE)
			_tprintf(ERROR_CMD, cmdTokenList[0]);
	}

	return 0;
}

/**********************************************************************
함수: TCHAR * StrLower (TCHAR *pStr)
기능: 문자열의 내에 존재하는 모든 대문자를 소문자로 변경한다.
	  변경된 문자열의 포인터를 반환한다.
**********************************************************************/
TCHAR * StrLower(TCHAR * pStr)
{
	TCHAR * ret = pStr;

	while (*pStr)
	{
		if (_istupper(*pStr))
			*pStr = _totlower(*pStr);
		pStr++;
	}

	return ret;
}
