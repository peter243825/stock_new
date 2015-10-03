#include "StdAfx.h"
#include "StockDataLoader.h"
#include "StockDataDownloader.h"
#include "utility.h"
#include "micro.h"
#include <algorithm>

using namespace std;

CStockDataLoader::CStockDataLoader(void)
{
}

CStockDataLoader::~CStockDataLoader(void)
{
}

size_t CStockDataLoader::LoadFromFile(const std::string& fileName, bool filterNoTradeData, bool getStockNameByFileName/* = true*/)
{
	string context;
	mStockData.dataVector.clear();
	mRowTitle.clear();
	if (ReadFromFile(fileName, context) <= 0)
	{
		return 0;
	}
	if (getStockNameByFileName)
	{
		vector<string> splitResult = StringSplitter(fileName, "\\");
		string tempString = *splitResult.rbegin();
		splitResult = StringSplitter(tempString, ".csv");
		mStockData.stockID = splitResult[0];
	}
	return LoadFromString(context, filterNoTradeData);
}

size_t CStockDataLoader::LoadFromString(const std::string& dataStr, bool filterNoTradeData)
{
	mStockData.dataVector.clear();
	mRowTitle.clear();
	vector<string> splitResult = StringSplitter(dataStr, "\n");
	if (splitResult.size() <= 1) //bad split
	{
		return 0;
	}
	vector<string> tempRowTitle = StringSplitter(splitResult[0], ",");
	if (tempRowTitle.size() <= 1) //bad split
	{
		return 0;
	}
	mRowTitle.swap(tempRowTitle);
	size_t i;
	for (i = 1; i < splitResult.size(); ++i)
	{
		StockDataItem item(mRowTitle, splitResult[i]);
		if (filterNoTradeData && item.IsNoTradeData())
		{
			continue;
		}
		mStockData.dataVector.push_back(item);
	}
	sort(mStockData.dataVector.begin(), mStockData.dataVector.end(), StockDataItem::LaterThan);
	return mStockData.dataVector.size();
}

StockData CStockDataLoader::GetStockData()
{
	return mStockData;
}

void CStockDataLoader::Clean()
{
	mRowTitle.clear();
	mStockData.stockID.clear();
	mStockData.dataVector.clear();
}

CStockListLoader::CStockListLoader(void)
{}

CStockListLoader::~CStockListLoader(void)
{}

size_t CStockListLoader::LoadFromFile(const std::string& fileName)
{
	string context;
	Clean();
	if (ReadFromFile(fileName, context) <= 0)
	{
		return 0;
	}
	vector<string> splitResult = StringSplitter(context, "\n");
	if (splitResult.size() <= 1) //bad split
	{
		return 0;
	}
	vector<string> tempRowTitle = StringSplitter(splitResult[0], ",");
	if (tempRowTitle.size() <= 1) //bad split
	{
		return 0;
	}
	mRowTitle.swap(tempRowTitle);
	size_t i;
	for (i = 1; i < splitResult.size(); ++i)
	{
		StockListItem item(mRowTitle, splitResult[i]);
		mStockList.push_back(item);
	}
	sort(mStockList.begin(), mStockList.end());
	return mStockList.size();
}

StockList CStockListLoader::GetStockList()
{
	return mStockList;
}

void CStockListLoader::Clean()
{
	mStockList.clear();
	mRowTitle.clear();
}