#pragma once
#include "base_type.h"

class CStockDataLoader
{
public:
	CStockDataLoader(void);
	~CStockDataLoader(void);
	size_t LoadFromFile(const std::string& fileName, bool filterNoTradeData, bool getStockNameByFileName = true);
	size_t LoadFromString(const std::string& dataStr, bool filterNoTradeData);
	StockData GetStockData();
	void Clean();
private:
	StockData mStockData;
	std::vector<std::string> mRowTitle;
};

class CStockListLoader
{
public:
	CStockListLoader(void);
	~CStockListLoader(void);
	size_t LoadFromFile(const std::string& fileName);
	StockList GetStockList();
	void Clean();
private:
	StockList mStockList;
	std::vector<std::string> mRowTitle;
};