// china_stock_analysis_v2.cpp : �������̨Ӧ�ó������ڵ㡣
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


// Ψһ��Ӧ�ó������

CWinApp theApp;

using namespace std;

void singleAnalysisMode()
{
	PrintLog("���ɷ���");
	std::string stockID, selectedMonth;
	std::cout << "�������Ʊ���루���磺600295��002362����";
	std::cin >> stockID;
	std::cout << "��������Ҫģ����·ݣ����磺201509����";
	std::cin >> selectedMonth;
	CAnalysisEngine::SingleStockAnalysis(stockID, selectedMonth);
	PrintLog("���ɷ�������\n");
}

void batchAnalysisMode()
{
	PrintLog("��������");
	DEFINE_TEMP_BUFFER(tempString);
	size_t i;
	std::string context, selectedMonth;
	StockList stockList;
	if (ReadFromFile(BatchAnalysisFile, context) <= 0)
	{
		PrintLog("��ȡ�ļ�����");
		return;
	}
	vector<string> splitResult = StringSplitter(context, "\n");
	if (splitResult.size() < 1) //bad split
	{
		PrintLog("��ȡ�ļ�����");
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
		PrintLog("��Ʊ�б�Ϊ��");
		return;
	}
	sprintf_s(tempString, "��Ʊ�б��ȣ�%d", stockList.size());
	PrintLog(tempString);

	std::cout << "��������Ҫģ����·ݣ����磺201509����";
	std::cin >> selectedMonth;
	sprintf_s(tempString, "%s�������������\n", selectedMonth.c_str());
	context = string(tempString);
	for (i = 0; i < stockList.size(); ++i)
	{
		StockListItem item = stockList[i];
		std::string result = CAnalysisEngine::SingleStockAnalysis(item.stockID, selectedMonth);
		context += (item.stockID + "\t" + item.stockName + "\t" + result + "\n");
	}
	WriteToFile(BatchAnalysisResultFile, context);
	PrintLog("������������\n");
}

void singlePredictMode()
{
	PrintLog("����Ԥ��");
	std::string stockID;
	std::cout << "�������Ʊ���루���磺600295��002362����";
	std::cin >> stockID;
	CAnalysisEngine::SingleStockPredict(stockID);
	PrintLog("����Ԥ�����\n");
}

void batchPredictMode()
{
	PrintLog("����Ԥ��");
	DEFINE_TEMP_BUFFER(tempString);
	size_t i;
	std::string context;
	StockList stockList;
	if (ReadFromFile(BatchPredictFile, context) <= 0)
	{
		PrintLog("��ȡ�ļ�����");
		return;
	}
	vector<string> splitResult = StringSplitter(context, "\n");
	if (splitResult.size() < 1) //bad split
	{
		PrintLog("��ȡ�ļ�����");
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
		PrintLog("��Ʊ�б�Ϊ��");
		return;
	}
	sprintf_s(tempString, "��Ʊ�б��ȣ�%d", stockList.size());
	PrintLog(tempString);

	//�����ָ֤��������ȡ�����һ�������յ�ʱ��
	DateType beginDate = DateType::GetDate(0);
	beginDate.month -= 1;
	beginDate.Adjust();
	DateType endDate = DateType::GetDate(0);
	CStockDataDownloader downloader;
	bool ret = downloader.Download(ShangHaiStockMarketID, beginDate, endDate);
	if (!ret)
	{
		PrintLog("��������ʧ��");
		return;
	}
	std::string fileName = CacheDataDir + ShangHaiStockMarketID+ ".csv";
	CStockDataLoader loader;
	size_t size = loader.LoadFromFile(fileName, true);
	if (size <= 0)
	{
		PrintLog("��ȡ����ʧ��");
		return;
	}
	StockData data = loader.GetStockData();
	sprintf_s(tempString, "%s����һ������Ԥ����\n", data.dataVector.rbegin()->date.ToString().c_str());
	context = string(tempString);
	for (i = 0; i < stockList.size(); ++i)
	{
		StockListItem item = stockList[i];
		std::string result = CAnalysisEngine::SingleStockPredict(item.stockID);
		context += (item.stockID + "\t" + item.stockName + "\t" + result + "\n");
	}
	WriteToFile(BatchPredictsResultFile, context);
	PrintLog("����Ԥ�����\n");
}

void start()
{
	PrintLog("����ʼ����");
	while (true)
	{
		std::string selectedMode;
		std::cout << "��ѡ������ģʽ��\n\n1. ���ɷ���\n2. ��������\n3. ������һ������Ԥ��\n4. ������һ������Ԥ��\n\n10. �˳�����\n\n�����빦�ܴ��루1~10����";
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
			std::cout << "δʶ���ģʽ��������ѡ��\n\n";
		}
	}
	PrintLog("�������н���");
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