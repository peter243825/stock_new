// china_stock_analysis_v2.cpp : 定义控制台应用程序的入口点。
//

#include "stdafx.h"
#include "china_stock_analysis_v2.h"
#include "StockDataDownloader.h"
#include <iostream>
#include <string>
#include "StockDataLoader.h"
#include "AnalysisEngine.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif


// 唯一的应用程序对象

CWinApp theApp;

using namespace std;

void singleAnalysisMode()
{
	PrintLog("个股分析");
	std::string stockID, selectedMonth;
	std::cout << "请输入股票代码（例如：600295、002362）：";
	std::cin >> stockID;
	std::cout << "请输入需要模拟的月份（例如：201509）：";
	std::cin >> selectedMonth;
	CAnalysisEngine::SingleStockAnalysis(stockID, selectedMonth);
	PrintLog("个股分析结束\n");
}

void batchAnalysisMode()
{
	PrintLog("批量分析");
	DEFINE_TEMP_BUFFER(tempString);
	size_t i;
	std::string context, selectedMonth;
	StockList stockList;
	if (ReadFromFile(BatchAnalysisFile, context) <= 0)
	{
		PrintLog("读取文件错误");
		return;
	}
	vector<string> splitResult = StringSplitter(context, "\n");
	if (splitResult.size() < 1) //bad split
	{
		PrintLog("读取文件错误");
		return;
	}
	for (i = 0; i < splitResult.size(); ++i)
	{
		vector<string> subSplitResult = StringSplitter(splitResult[i], "\t");
		if (subSplitResult.size() <= 1) //bad split
		{
			continue;
		}
		if (subSplitResult.size() > 2) //there is one or more space in stock's name, merge them
		{
			size_t j;
			for (j = 2; j < subSplitResult.size(); ++j)
			{
				subSplitResult[1] += (string(" ") + subSplitResult[j]);
			}
		}
		StockListItem item;
		item.stockID = subSplitResult[0];
		item.stockName = subSplitResult[1];
		stockList.push_back(item);
	}
	if (stockList.empty())
	{
		PrintLog("股票列表为空");
		return;
	}
	sprintf_s(tempString, "股票列表长度：%d", stockList.size());
	PrintLog(tempString);

	std::cout << "请输入需要模拟的月份（例如：201509）：";
	std::cin >> selectedMonth;
	sprintf_s(tempString, "%s的批量分析结果\n", selectedMonth.c_str());
	context = string(tempString);
	for (i = 0; i < stockList.size(); ++i)
	{
		StockListItem item = stockList[i];
		std::string result = CAnalysisEngine::SingleStockAnalysis(item.stockID, selectedMonth);
		context += (item.stockID + "\t" + item.stockName + "\t" + result + "\n");
	}
	WriteToFile(BatchAnalysisResultFile, context);
	PrintLog("批量分析结束\n");
}

void singlePredictMode()
{
	PrintLog("个股预测");
	std::string stockID;
	std::cout << "请输入股票代码（例如：600295、002362）：";
	std::cin >> stockID;
	CAnalysisEngine::SingleStockPredict(stockID);
	PrintLog("个股预测结束\n");
}

void batchPredictMode()
{
	PrintLog("批量预测");
	DEFINE_TEMP_BUFFER(tempString);
	size_t i;
	std::string context;
	StockList stockList;
	if (ReadFromFile(BatchPredictFile, context) <= 0)
	{
		PrintLog("读取文件错误");
		return;
	}
	vector<string> splitResult = StringSplitter(context, "\n");
	if (splitResult.size() < 1) //bad split
	{
		PrintLog("读取文件错误");
		return;
	}
	for (i = 0; i < splitResult.size(); ++i)
	{
		vector<string> subSplitResult = StringSplitter(splitResult[i], "\t");
		if (subSplitResult.size() <= 1) //bad split
		{
			continue;
		}
		if (subSplitResult.size() > 2) //there is one or more space in stock's name, merge them
		{
			size_t j;
			for (j = 2; j < subSplitResult.size(); ++j)
			{
				subSplitResult[1] += (string(" ") + subSplitResult[j]);
			}
		}
		StockListItem item;
		item.stockID = subSplitResult[0];
		item.stockName = subSplitResult[1];
		stockList.push_back(item);
	}
	if (stockList.empty())
	{
		PrintLog("股票列表为空");
		return;
	}
	sprintf_s(tempString, "股票列表长度：%d", stockList.size());
	PrintLog(tempString);

	//获得上证指数数据以取得最后一个交易日的时间
	DateType beginDate = DateType::GetDate(0);
	beginDate.month -= 1;
	beginDate.Adjust();
	DateType endDate = DateType::GetDate(0);
	CStockDataDownloader downloader;
	bool ret = downloader.Download(ShangHaiStockMarketID, beginDate, endDate);
	if (!ret)
	{
		PrintLog("数据下载失败");
		return;
	}
	std::string fileName = CacheDataDir + ShangHaiStockMarketID+ ".csv";
	CStockDataLoader loader;
	size_t size = loader.LoadFromFile(fileName, true);
	if (size <= 0)
	{
		PrintLog("读取数据失败");
		return;
	}
	StockData data = loader.GetStockData();
	sprintf_s(tempString, "%s的下一交易日预测结果\n", data.dataVector.rbegin()->date.ToString().c_str());
	context = string(tempString);
	for (i = 0; i < stockList.size(); ++i)
	{
		StockListItem item = stockList[i];
		std::string result = CAnalysisEngine::SingleStockPredict(item.stockID);
		context += (item.stockID + "\t" + item.stockName + "\t" + result + "\n");
	}
	WriteToFile(BatchPredictsResultFile, context);
	PrintLog("批量预测结束\n");
}

void start()
{
	PrintLog("程序开始启动");
	while (true)
	{
		std::string selectedMode;
		std::cout << "请选择运行模式：\n\n1. 个股分析\n2. 批量分析\n3. 个股下一交易日预测\n4. 批量下一交易日预测\n\n10. 退出程序\n\n请输入功能代码（1~10）：";
		std::cin >> selectedMode;
		if (selectedMode == "1")
		{
			singleAnalysisMode();
		}
		else if (selectedMode == "2")
		{
			batchAnalysisMode();
		}
		else if (selectedMode == "3")
		{
			singlePredictMode();
		}
		else if (selectedMode == "4")
		{
			batchPredictMode();
		}
		else if (selectedMode == "10")
		{
			break;
		}
		else
		{
			std::cout << "未识别的模式，请重新选择\n\n";
		}
	}
	PrintLog("程序运行结束");
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