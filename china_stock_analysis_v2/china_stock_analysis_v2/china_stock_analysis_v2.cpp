// china_stock_analysis_v2.cpp : �������̨Ӧ�ó������ڵ㡣
//

#include "stdafx.h"
#include "china_stock_analysis_v2.h"
#include <iostream>
#include <string>

#ifdef _DEBUG
#define new DEBUG_NEW
#endif


// Ψһ��Ӧ�ó������

CWinApp theApp;

using namespace std;

void start()
{
	std::string stockName, selectedMonth;
	std::cout << "�������Ʊ���루���磺600295��002362����";
	std::cin >> stockName;
	std::cout << "��������Ҫģ����·ݣ����磺201509����";
	std::cin >> selectedMonth;
	system("pause");
}

int _tmain(int argc, TCHAR* argv[], TCHAR* envp[])
{
	int nRetCode = 0;

	HMODULE hModule = ::GetModuleHandle(NULL);

	if (hModule != NULL)
	{
		// ��ʼ�� MFC ����ʧ��ʱ��ʾ����
		if (!AfxWinInit(hModule, NULL, ::GetCommandLine(), 0))
		{
			// TODO: ���Ĵ�������Է���������Ҫ
			_tprintf(_T("����: MFC ��ʼ��ʧ��\n"));
			nRetCode = 1;
		}
		else
		{
			start();
		}
	}
	else
	{
		// TODO: ���Ĵ�������Է���������Ҫ
		_tprintf(_T("����: GetModuleHandle ʧ��\n"));
		nRetCode = 1;
	}
	return nRetCode;
}