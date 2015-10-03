// china_stock_analysis_v2.cpp : 定义控制台应用程序的入口点。
//

#include "stdafx.h"
#include "china_stock_analysis_v2.h"
#include <iostream>
#include <string>

#ifdef _DEBUG
#define new DEBUG_NEW
#endif


// 唯一的应用程序对象

CWinApp theApp;

using namespace std;

void start()
{
	std::string stockName, selectedMonth;
	std::cout << "请输入股票代码（例如：600295、002362）：";
	std::cin >> stockName;
	std::cout << "请输入需要模拟的月份（例如：201509）：";
	std::cin >> selectedMonth;
	system("pause");
}

int _tmain(int argc, TCHAR* argv[], TCHAR* envp[])
{
	int nRetCode = 0;

	HMODULE hModule = ::GetModuleHandle(NULL);

	if (hModule != NULL)
	{
		// 初始化 MFC 并在失败时显示错误
		if (!AfxWinInit(hModule, NULL, ::GetCommandLine(), 0))
		{
			// TODO: 更改错误代码以符合您的需要
			_tprintf(_T("错误: MFC 初始化失败\n"));
			nRetCode = 1;
		}
		else
		{
			start();
		}
	}
	else
	{
		// TODO: 更改错误代码以符合您的需要
		_tprintf(_T("错误: GetModuleHandle 失败\n"));
		nRetCode = 1;
	}
	return nRetCode;
}