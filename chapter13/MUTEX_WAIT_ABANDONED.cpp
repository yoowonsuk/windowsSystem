#include<cstdio>
#include<tchar.h>
#include<Windows.h>
#include<process.h>
 
LONG gTotalCount = 0;
HANDLE hMutex;
 
unsigned int WINAPI IncreaseCountOne(LPVOID lpParam){
    WaitForSingleObject(hMutex, INFINITE);
    gTotalCount++;
    puts("1st thread");
    //ReleaseMutex(hMutex);
    return 0;
}
 
unsigned int WINAPI IncreaseCountTwo(LPVOID lpParam){
    DWORD dwWaitResult = 0;
    dwWaitResult = WaitForSingleObject(hMutex, INFINITE);
 
    switch (dwWaitResult){
    case WAIT_OBJECT_0:
        ReleaseSemaphore(hSemaphore, 1, NULL);
        break;
    case WAIT_ABANDONED:
        printf("wait_abandoned\n");
        break;
    }
 
    gTotalCount++;
    ReleaseMutex(hMutex);
    return 0;
}
 
int main(){
    DWORD dwThreadIDOne;
    DWORD dwThreadIDTwo;
 
    HANDLE hThreadOne;
    HANDLE hThreadTwo;
 
    hMutex = CreateMutex(NULL, false, NULL);
 
    hThreadOne = (HANDLE)_beginthreadex(
        NULL, 0, IncreaseCountOne, NULL,
        0, NULL);
    hThreadTwo = (HANDLE)_beginthreadex(
        NULL, 0, IncreaseCountTwo, NULL,
        0, NULL);
 
    Sleep(100);
    ResumeThread(hThreadTwo);
 
    WaitForSingleObject(hThreadTwo, INFINITE);
    printf("total cnt : %d\n", gTotalCount);
 
    CloseHandle(hThreadOne);
    CloseHandle(hThreadTwo);
 
    return 0;
}


출처: https://blog.jkns.kr/521 [Celestial Link]
