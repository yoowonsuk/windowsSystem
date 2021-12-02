

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

int _tmain(int argc, TCHAR * argv[])
{
	SortString();

	return 0;
}
