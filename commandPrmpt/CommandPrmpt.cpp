/*
    CommandPrmpt_Six.cpp
    프로그램 설명: 명령 프롬프트 6차 
    명령어 1: type --> 텍스트 파일 내용 출력.
    명령어 2: type text.txt | sort  --> 파이프.
*/

#include <stdio.h>
#include <tchar.h>
#include <locale.h>
#include <windows.h> 
#include <tlhelp32.h>
#include <ctype.h>

#define STR_LEN    256
#define CMD_TOKEN_NUM  10

TCHAR ERROR_CMD[] 
= _T("'%s'은(는) 실행할 수 있는 프로그램이 아닙니다. \n");

TCHAR cmdString[STR_LEN];
TCHAR cmdTokenList[CMD_TOKEN_NUM][STR_LEN];
TCHAR seps[]   = _T(" ,\t\n");

int CmdReadTokenize(void);
int CmdProcessing(int);
TCHAR * StrLower(TCHAR *);

int _tmain(int argc, TCHAR * argv[])
{
	// 한글 입력을 가능케 하기 위해.
	_tsetlocale(LC_ALL, _T("Korean")); 

	if(argc > 2)  // 매개변수 전달인자가 있는 경우.
	{
		for(int i= 1; i<argc; i++)
			_tcscpy(cmdTokenList[i-1], argv[i]);

		CmdProcessing(argc-1);
	}

	DWORD isExit = 0; //NULL;
	while(1)
	{
		int tokenNum = CmdReadTokenize();
		if(tokenNum == 0) //Enter 입력시 처리를 위해...
			continue;

		isExit = CmdProcessing(tokenNum);
		if(isExit == TRUE)
		{
			_fputts(_T("명령어 처리를 종료합니다. \n"), stdout );
			break;
		}
	}

	return 0; 
}

int CmdReadTokenize(void)
{
	TCHAR * token;

	_fputts( _T("Best command prompt>> "), stdout );
	_getts(cmdString);

	token = _tcstok(cmdString, seps);

	int tokenNum = 0;

	while(token != NULL)
	{
		_tcscpy ( cmdTokenList[tokenNum++], StrLower(token) );
		token = _tcstok(NULL, seps);
	}

	return tokenNum;
}

void ListProcessInfo(void)
{
	HANDLE hProcessSnap = CreateToolhelp32Snapshot( TH32CS_SNAPPROCESS, 0 );
	if( hProcessSnap == INVALID_HANDLE_VALUE )
	{
		_tprintf( _T("CreateToolhelp32Snapshot error! \n") );
		return;
	}

	PROCESSENTRY32 pe32;	/* 프로세스 정보 얻기위한 구조체 변수 */

	/* PROCESSENTRY32 변수는 사용하기 전에 크기 정보 초기화 해야한다 */
	pe32.dwSize = sizeof( PROCESSENTRY32 );

	/* 첫 번째 프로세스 정보 얻음 */
	if( !Process32First( hProcessSnap, &pe32 ) )
	{
		_tprintf( _T("Process32First error! \n") );
		CloseHandle( hProcessSnap );
		return; 
	}

	do
	{
		/* 프로세스 이름, ID 정보 출력 */
		_tprintf(_T("%25s %5d \n"), pe32.szExeFile, pe32.th32ProcessID); 

	} while( Process32Next( hProcessSnap, &pe32 ) );

	CloseHandle( hProcessSnap );
}

void KillProcess(void)
{
	HANDLE hProcessSnap = CreateToolhelp32Snapshot( TH32CS_SNAPPROCESS, 0 );
	if( hProcessSnap == INVALID_HANDLE_VALUE )
	{
		_tprintf( _T("CreateToolhelp32Snapshot (of processes)") );
		return;
	}

	PROCESSENTRY32 pe32;
	pe32.dwSize = sizeof( PROCESSENTRY32 );

	if( !Process32First( hProcessSnap, &pe32 ) )
	{
		_tprintf( _T("Process32First") );
		CloseHandle( hProcessSnap );
		return;
	}

	HANDLE hProcess;
	BOOL isKill = FALSE;
	do
	{
		if(_tcscmp(pe32.szExeFile, cmdTokenList[1]) == 0)
		{
			hProcess = OpenProcess(PROCESS_ALL_ACCESS, FALSE, pe32.th32ProcessID );

			if(hProcess != NULL)
			{
				TerminateProcess(hProcess, -1);
				isKill = TRUE;
			}

			CloseHandle(hProcess);
			break;
		}

	} while( Process32Next( hProcessSnap, &pe32 ) );

	CloseHandle( hProcessSnap );

	if(isKill == FALSE)
		_tprintf(_T("Kill process fail, Try again! \n"));
}

void TypeTextFile(void)
{
	TCHAR cmdStringWithOptions[STR_LEN]={0,};
	BOOL isRun;

	if(!_tcscmp(cmdTokenList[2], _T("|")) )
	{
		/* Create unnamed pipe */
		HANDLE hReadPipe, hWritePipe;

		SECURITY_ATTRIBUTES pipeSA ={
			sizeof(SECURITY_ATTRIBUTES), NULL, TRUE
		};

		CreatePipe(&hReadPipe, &hWritePipe, &pipeSA, 0);

		/* process type을 위한 선언 */
		STARTUPINFO siType={0,};
		PROCESS_INFORMATION piType;
		siType.cb=sizeof(siType);

		siType.hStdInput=GetStdHandle(STD_INPUT_HANDLE);
		siType.hStdError=GetStdHandle(STD_ERROR_HANDLE);
		siType.hStdOutput=hWritePipe;       // 출력 리다이렉션!
		siType.dwFlags |=STARTF_USESTDHANDLES;

		_tcscpy(cmdStringWithOptions, cmdTokenList[0]);
		_stprintf(cmdStringWithOptions, _T("%s %s"), cmdStringWithOptions, cmdTokenList[1]);

		isRun = CreateProcess(NULL, cmdStringWithOptions, NULL, NULL, TRUE, 0, NULL, NULL, &siType, &piType);

		CloseHandle(piType.hThread);
		CloseHandle(hWritePipe);

		/* process sort를 위한 선언*/
		STARTUPINFO siSort={0,};
		PROCESS_INFORMATION piSort;
		siSort.cb=sizeof(siSort);

		siSort.hStdInput=hReadPipe;		    // 입력 리다이렉션!
		siSort.hStdError=GetStdHandle(STD_ERROR_HANDLE);
		siSort.hStdOutput=GetStdHandle(STD_OUTPUT_HANDLE);
		siSort.dwFlags |=STARTF_USESTDHANDLES;

		isRun = CreateProcess(NULL, cmdTokenList[3], NULL, NULL, TRUE, 0, NULL, NULL, &siSort, &piSort);

		CloseHandle(piSort.hThread);
		CloseHandle(hReadPipe);

		WaitForSingleObject(piType.hProcess, INFINITE);
		WaitForSingleObject(piSort.hProcess, INFINITE);

		CloseHandle(piType.hProcess);
		CloseHandle(piSort.hProcess);
	}
	else
	{
		_tcscpy(cmdStringWithOptions, cmdTokenList[0]);
		_stprintf(cmdStringWithOptions, _T("%s %s"), cmdStringWithOptions, cmdTokenList[1]);

		STARTUPINFO si={0,};
		PROCESS_INFORMATION pi;
		si.cb=sizeof(si);

		isRun = CreateProcess(NULL, cmdStringWithOptions, NULL, NULL, FALSE, 0, NULL, NULL, &si, &pi);
		WaitForSingleObject(pi.hProcess, INFINITE);

		CloseHandle(pi.hProcess);
		CloseHandle(pi.hThread);
	}
}

int CmdProcessing(int tokenNum)
{ 
	BOOL isRun;
	STARTUPINFO si={0,};
	PROCESS_INFORMATION pi; 

	TCHAR cmdStringWithOptions[STR_LEN]={0,};
	TCHAR optString[STR_LEN]={0,};

	si.cb=sizeof(si);
	if( !_tcscmp(cmdTokenList[0],_T("exit")) )
	{  
		return TRUE;
	}
	else if ( !_tcscmp(cmdTokenList[0],_T("start")) )
	{
		if(tokenNum >1)
		{
			for(int i=1; i<tokenNum; i++)
				_stprintf(optString, _T("%s %s"), optString, cmdTokenList[i]);

			_stprintf(cmdStringWithOptions, _T("%s %s"), _T("CmdProject.exe"), optString);
		}
		else
			_stprintf(cmdStringWithOptions, _T("%s"), _T("CmdProject.exe"));

		isRun = CreateProcess(NULL, cmdStringWithOptions, NULL, NULL, TRUE, CREATE_NEW_CONSOLE, NULL, NULL, &si, &pi);

		CloseHandle(pi.hProcess);
		CloseHandle(pi.hThread);
	}
	else if ( !_tcscmp(cmdTokenList[0], _T("echo")) )
	{
		for(int i=1; i<tokenNum; i++)
			_stprintf(optString, _T("%s %s"), optString, cmdTokenList[i]);

		_tprintf( _T("echo message:%s \n"), optString);
	}
	else if( !_tcscmp(cmdTokenList[0], _T("lp")) )
	{
		ListProcessInfo();
	}
	else if( !_tcscmp(cmdTokenList[0], _T("kp")) )
	{
		if(tokenNum <2)
		{
			_tprintf(_T("usage: kp <process name> \n"));
			return 0; 
		}

		KillProcess();
	}
	else if( !_tcscmp(cmdTokenList[0], _T("sort")) )
	{
		if(!_tcscmp(cmdTokenList[1], _T(">")) )
		{
			SECURITY_ATTRIBUTES fileSec = {
				sizeof(SECURITY_ATTRIBUTES), NULL, TRUE
			};// 상속 가능해야 지정 가능!

			HANDLE hFile = CreateFile ( 
				cmdTokenList[2], GENERIC_WRITE,	FILE_SHARE_READ,
				&fileSec, CREATE_ALWAYS, FILE_ATTRIBUTE_NORMAL,	NULL
			);

			si.hStdOutput = hFile;
			si.hStdInput = GetStdHandle(STD_INPUT_HANDLE);
			si.hStdError = GetStdHandle(STD_ERROR_HANDLE);
			si.dwFlags |= STARTF_USESTDHANDLES;

			isRun = CreateProcess(NULL, cmdTokenList[0], NULL, NULL, TRUE, 0, NULL, NULL, &si, &pi);

			WaitForSingleObject(pi.hProcess, INFINITE);
			CloseHandle(hFile);
		}
		else
		{
			isRun = CreateProcess(NULL, cmdTokenList[0], NULL, NULL, FALSE, 0, NULL, NULL, &si, &pi);
			WaitForSingleObject(pi.hProcess, INFINITE);
		}

		CloseHandle(pi.hProcess);
		CloseHandle(pi.hThread);

	}
	/*else if( !_tcscmp(cmdTokenList[0], _T("type")) )
	{
		TypeTextFile();
	}
	*/
	else
	{
		_tcscpy(cmdStringWithOptions, cmdTokenList[0]);

		for(int i=1; i<tokenNum; i++)
			_stprintf(cmdStringWithOptions, _T("%s %s"), cmdStringWithOptions, cmdTokenList[i]);

		isRun = CreateProcess(NULL, cmdStringWithOptions, NULL, NULL, TRUE, 0, NULL, NULL, &si, &pi);

		CloseHandle(pi.hProcess);
		CloseHandle(pi.hThread);

		if(isRun == FALSE)
			_tprintf(ERROR_CMD, cmdTokenList[0]);
	}

	return 0;
}


TCHAR * StrLower(TCHAR *pStr)
{
	TCHAR *ret = pStr;

	while(*pStr)
	{
		if(_istupper(*pStr))
			*pStr = _totlower(*pStr);

		pStr++;
	}

	return ret;
}



/*************************************************************************************
sort는 프로세스로 생성....
**************************************************************************************/
#define MAX_STRING_NUM 100
#define MAX_STRING_LEN 256

void SortString()  // 파이프 예제를 위해서 독립된 실행파일로 만들어야 한다(힌트).
{
	TCHAR stringArr[MAX_STRING_NUM][MAX_STRING_LEN];

	// 콘솔로부터 문자열을 읽어들인다.
	int nStr;
	for(nStr=0; nStr<MAX_STRING_NUM; nStr++)
	{
		TCHAR * isEOF = _fgetts(stringArr[nStr], MAX_STRING_LEN, stdin);
		if(isEOF == NULL)    // EOF는 Ctrl+Z..
			break;
	}

	// String Bubble Sort... 성능 고려하지 않고 문자열 단위 연산...
	TCHAR strTemp[MAX_STRING_LEN];

	for(int i=0; i<nStr; i++)
	{
		for(int j= nStr-1; j>i; j--)
		{
			if( _tcscmp(stringArr[j-1], stringArr[j]) >0 )
			{
				_tcscpy(strTemp, stringArr[j-1]);
				_tcscpy(stringArr[j-1], stringArr[j]);
				_tcscpy(stringArr[j], strTemp); 
			}
		}
	}

	for(int i=0; i<nStr; i++)
		_fputts(stringArr[i], stdout);
}

int _tmain_sortmain(int argc, TCHAR * argv[])
{
	SortString();

	return 0;
}

/*************************************************************************************
type은 프로세스로 생성....
**************************************************************************************/
void TYPE(TCHAR * fileName)	// FOR REDIRECTION...
{
	static TCHAR StringBuff[1024];

	FILE * filePtr = _tfopen(fileName, _T("rt"));
	while(_fgetts(StringBuff, 1024, filePtr))
		_fputts(StringBuff, stdout);
}

int _tmain_typemain(int argc, TCHAR * argv[])
{
	if(argc <2)
		return -1; 

	TYPE(argv[1]);

	return 0;
}

