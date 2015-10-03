// china_stock_analysis_v2.cpp : 定义控制台应用程序的入口点。
//

#include "stdafx.h"
#include "china_stock_analysis_v2.h"
#include "StockDataDownloader.h"
#include <iostream>
#include <string>
#include "StockDataLoader.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif


// 唯一的应用程序对象

CWinApp theApp;

using namespace std;

void start()
{
	PrintLog("程序开始启动");
	std::string stockID, selectedMonth;
	DEFINE_TEMP_BUFFER(tempString);
	std::cout << "请输入股票代码（例如：600295、002362）：";
	std::cin >> stockID;
	std::cout << "请输入需要模拟的月份（例如：201509）：";
	std::cin >> selectedMonth;
	PrintLog(std::string("股票代码：") + stockID + "，模拟月份：" + selectedMonth);

	//计算日期区间（开始时间为模拟月份减2的1号，结束时间为模拟月份的最后一天）
	int year = atoi(selectedMonth.substr(0, 4).c_str());
	int month = atoi(selectedMonth.substr(4).c_str());
	DateType beginDate(year, month - 2, 1);
	beginDate.Adjust();
	DateType endDate(year, month + 1, 0);
	endDate.Adjust();
	DateType lastMonth(year, month - 1, 1);
	DateType selectedTime(year, month, 1);
	
	do 
	{
	PrintLog(std::string("下载数据：") + beginDate.ToString() + " -> " + endDate.ToString());
	CStockDataDownloader downloader;
	bool ret = downloader.Download(stockID, beginDate, endDate);
	if (!ret)
	{
		PrintLog("数据下载失败");
		break;
	}
	PrintLog("读取数据");
	std::string fileName = CacheDataDir + stockID + ".csv";
	CStockDataLoader loader;
	size_t size = loader.LoadFromFile(fileName, true);
	if (size <= 0)
	{
		PrintLog("读取数据失败");
		break;
	}
	StockData data = loader.GetStockData();
	sprintf_s(tempString, "数据量：%d", size);
	PrintLog(tempString);
	PrintLog("开始模拟");
	std::vector<StockDataItem>::iterator iter(data.dataVector.begin()), endIter(data.dataVector.end());
	for (; iter != endIter; ++iter)
	{
		if (DateType::IsSameMonth(iter->date, lastMonth))
		{
			break;
		}
	}
	if (iter == endIter)
	{
		PrintLog("没有找到上月K线数据");
		break;
	}
	double lowPriceInMonth = iter->lowPrice, highPriceInMonth = iter->highPrice, closePriceInMonth;
	for (; iter != endIter; ++iter)
	{
		if (!DateType::IsSameMonth(iter->date, lastMonth))
		{
			break;
		}
		if (lowPriceInMonth > iter->lowPrice)
		{
			lowPriceInMonth = iter->lowPrice;
		}
		if (highPriceInMonth < iter->highPrice)
		{
			highPriceInMonth = iter->highPrice;
		}
		closePriceInMonth = iter->closePrice;
	}
	double half = (highPriceInMonth + lowPriceInMonth) / 2;
	double deltaHigh = fabs(closePriceInMonth - highPriceInMonth);
	double deltaLow = fabs(closePriceInMonth - lowPriceInMonth);
	double deltaHalf = fabs(closePriceInMonth - half);
	std::string lastMonthState;
	if (deltaHigh < deltaLow)
	{
		if (deltaHigh < deltaHalf)
		{
			lastMonthState = "进攻局";
		}
		else
		{
			lastMonthState = "蓄势局";
		}
	}
	else
	{
		if (deltaLow < deltaHalf)
		{
			lastMonthState = "退守局";
		}
		else
		{
			lastMonthState = "蓄势局";
		}
	}
	sprintf_s(tempString, "上月K线盘局：最高 %.2lf，最低 %.2lf，月收盘价 %.2lf，%s", highPriceInMonth, lowPriceInMonth, closePriceInMonth, lastMonthState.c_str());
	PrintLog(tempString);
	if (iter == endIter)
	{
		PrintLog("没有找到选定月的K线数据");
		break;
	}
	double crucialHigh, crucialLow;
	if (lastMonthState == "退守局")
	{
		PrintLog("退守局不操作");
		break;
	}
	else if (lastMonthState == "进攻局")
	{
		crucialHigh = highPriceInMonth;
		crucialLow = iter->lowPrice;
	} 
	else
	{
		crucialHigh = iter->highPrice;
		crucialLow = iter->lowPrice;
	}
	sprintf_s(tempString, "选定月的关键区域：下限 %.2lf，上限 %.2lf", crucialLow, crucialHigh);
	PrintLog(tempString);
	data.GenerateEXPMADataVector(7);
	double buyInPrice = -1;
	double profit = 0;

	for (; iter != endIter; ++iter)
	{
		size_t expmaIndex = iter - data.dataVector.begin() - 6;
		double predictExpma;
		if (expmaIndex >= 2)
		{
			 predictExpma = data.expmaDataVector[expmaIndex - 1] * 2 - data.expmaDataVector[expmaIndex - 2];
		}
		else if (expmaIndex >= 1)
		{
			predictExpma = data.expmaDataVector[expmaIndex - 1];
		}
		else
		{
			predictExpma = -1000;
		}
		//买入信号：股价上涨，7日均线上扬，收盘在7日均线之上，收盘在关键区域之上
		if (buyInPrice < 0 && iter->closePrice > iter->openPrice && data.expmaDataVector[expmaIndex] > predictExpma && iter->closePrice > data.expmaDataVector[expmaIndex] && iter->closePrice > deltaHigh)
		{
			buyInPrice = (iter->closePrice + iter->openPrice) / 2;
			sprintf_s(tempString, "%s出现买入信号，买入价：%.2lf", iter->date.ToString().c_str(), buyInPrice);
		}
		//卖出信号：收盘在7日均线之下，或股价跌破关键区域
		if (buyInPrice > 0 && iter->closePrice < predictExpma && iter->closePrice < deltaHigh)
		{
			double sellOutPrice = (iter->closePrice + iter->openPrice) / 2;
			sprintf_s(tempString, "%s出现卖出信号，卖出价：%.2lf", iter->date.ToString().c_str(), sellOutPrice);
			profit += (sellOutPrice - buyInPrice) / buyInPrice;
			buyInPrice = -1;
		}
	}
	if (buyInPrice > 0)
	{
		profit += (data.dataVector.rbegin()->closePrice - buyInPrice) / buyInPrice;
	}
	sprintf_s(tempString, "模拟结束，该月总收益为：%.2lf%%", profit * 100);
	PrintLog("程序运行结束");
	} while (false);
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