#include "StdAfx.h"
#include "AnalysisEngine.h"
#include "utility.h"
#include "base_type.h"
#include "StockDataDownloader.h"
#include "StockDataLoader.h"
#include <stdio.h>


CAnalysisEngine::CAnalysisEngine(void)
{}


CAnalysisEngine::~CAnalysisEngine(void)
{}


std::string CAnalysisEngine::SingleStockAnalysis(const std::string& stockID, const std::string& selectedMonth)
{
	DEFINE_TEMP_BUFFER(tempString);
	std::string result;
	PrintLog(std::string("股票代码：") + stockID + "，模拟月份：" + selectedMonth);

	//计算日期区间（开始时间为模拟月份减2的1号，结束时间为模拟月份的最后一天）
	int year = atoi(selectedMonth.substr(0, 4).c_str());
	int month = atoi(selectedMonth.substr(4).c_str());
	DateType beginDate(year, month - 2, 1);
	beginDate.Adjust();
	DateType endDate(year, month + 1, 0);
	endDate.Adjust();
	DateType lastMonth(year, month - 1, 1);
	lastMonth.Adjust();
	DateType selectedTime(year, month, 1);

	do
	{
		PrintLog(std::string("下载数据：") + beginDate.ToString() + " -> " + endDate.ToString());
		CStockDataDownloader downloader;
		bool ret = downloader.Download(stockID, beginDate, endDate);
		if (!ret)
		{
			PrintLog("数据下载失败");
			result += "数据下载失败";
			break;
		}
		PrintLog("读取数据");
		std::string fileName = CacheDataDir + stockID + ".csv";
		CStockDataLoader loader;
		size_t size = loader.LoadFromFile(fileName, true);
		if (size <= 0)
		{
			PrintLog("读取数据失败");
			result += "读取数据失败";
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
			result += "没有找到上月K线数据";
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
		result += lastMonthState;
		if (iter == endIter)
		{
			PrintLog("没有找到本月的K线数据");
			result += " 没有找到本月的K线数据";
			break;
		}
		double crucialHigh, crucialLow;
		if (lastMonthState == "退守局")
		{
			result += " 退守局不操作";
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
		sprintf_s(tempString, "本月的关键区域：下限 %.2lf，上限 %.2lf", crucialLow, crucialHigh);
		result += (std::string(" ") + tempString);
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
			if (buyInPrice < 0 && iter->closePrice > iter->openPrice && data.expmaDataVector[expmaIndex] > predictExpma && iter->closePrice > data.expmaDataVector[expmaIndex] && iter->closePrice > crucialHigh)
			{
				buyInPrice = (iter->closePrice + iter->openPrice) / 2;
				sprintf_s(tempString, "%s出现买入信号，买入价：%.2lf", iter->date.ToString().c_str(), buyInPrice);
				PrintLog(tempString);
				result += (std::string(" ") + tempString);
			}
			//卖出信号：收盘在7日均线之下，或股价跌破关键区域
			if (buyInPrice > 0 && (iter->closePrice < predictExpma || iter->closePrice < crucialLow))
			{
				double sellOutPrice = (iter->closePrice + iter->openPrice) / 2;
				sprintf_s(tempString, "%s出现卖出信号，卖出价：%.2lf", iter->date.ToString().c_str(), sellOutPrice);
				PrintLog(tempString);
				result += (std::string(" ") + tempString);
				profit += (sellOutPrice - buyInPrice) / buyInPrice;
				buyInPrice = -1;
			}
		}
		if (buyInPrice > 0) //处于持仓状态
		{
			profit += (data.dataVector.rbegin()->closePrice - buyInPrice) / buyInPrice;
		}
		sprintf_s(tempString, "模拟结束，该月总收益为：%.2lf%%", profit * 100);
		PrintLog(tempString);
		if (DOUBLE_IS_ZERO(profit))
		{
			result += " 该月无操作";
		}
		else
		{
			sprintf_s(tempString, " 该月总收益为：%.2lf%%", profit * 100);
			result += tempString;
		}
	} while (false);
	return result;
}

std::string CAnalysisEngine::SingleStockPredict(const std::string& stockID)
{
	DEFINE_TEMP_BUFFER(tempString);
	std::string result;
	PrintLog(std::string("股票代码：") + stockID);

	//计算日期区间（开始时间为当前月份减2的1号，结束时间为当天）
	DateType beginDate = DateType::GetDate(0);
	beginDate.month -= 2;
	beginDate.date = 1;
	beginDate.Adjust();
	DateType endDate = DateType::GetDate(0);
	DateType lastMonth = DateType::GetDate(0);
	lastMonth.month--;
	lastMonth.Adjust();
	DateType selectedTime = beginDate;

	do
	{
		PrintLog(std::string("下载数据：") + beginDate.ToString() + " -> " + endDate.ToString());
		CStockDataDownloader downloader;
		bool ret = downloader.Download(stockID, beginDate, endDate);
		if (!ret)
		{
			PrintLog("数据下载失败");
			result += "数据下载失败";
			break;
		}
		PrintLog("读取数据");
		std::string fileName = CacheDataDir + stockID + ".csv";
		CStockDataLoader loader;
		size_t size = loader.LoadFromFile(fileName, true);
		if (size <= 0)
		{
			PrintLog("读取数据失败");
			result += "读取数据失败";
			break;
		}
		StockData data = loader.GetStockData();
		sprintf_s(tempString, "数据量：%d", size);
		PrintLog(tempString);
		PrintLog("开始预测");
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
			result += "没有找到上月K线数据";
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
		result += lastMonthState;
		if (iter == endIter)
		{
			PrintLog("没有找到本月的K线数据");
			result += " 没有找到本月的K线数据";
			break;
		}
		double crucialHigh, crucialLow;
		if (lastMonthState == "退守局")
		{
			result += " 退守局不操作";
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
		sprintf_s(tempString, "本月的关键区域：下限 %.2lf，上限 %.2lf", crucialLow, crucialHigh);
		result += (std::string(" ") + tempString);
		PrintLog(tempString);
		data.GenerateEXPMADataVector(7);
		double buyInPrice = -1;

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
			if (buyInPrice < 0 && iter->closePrice > iter->openPrice && data.expmaDataVector[expmaIndex] > predictExpma && iter->closePrice > data.expmaDataVector[expmaIndex] && iter->closePrice > crucialHigh)
			{
				buyInPrice = (iter->closePrice + iter->openPrice) / 2;
				sprintf_s(tempString, "%s出现买入信号，买入价：%.2lf", iter->date.ToString().c_str(), buyInPrice);
				PrintLog(tempString);
				result += (std::string(" ") + tempString);
			}
			//卖出信号：收盘在7日均线之下，或股价跌破关键区域
			if (buyInPrice > 0 && (iter->closePrice < predictExpma || iter->closePrice < crucialLow))
			{
				double sellOutPrice = (iter->closePrice + iter->openPrice) / 2;
				sprintf_s(tempString, "%s出现卖出信号，卖出价：%.2lf", iter->date.ToString().c_str(), sellOutPrice);
				PrintLog(tempString);
				result += (std::string(" ") + tempString);
				buyInPrice = -1;
			}
		}
		if (buyInPrice > 0) //当前持仓，计算卖出信号
		{
			size_t expmaIndex = data.dataVector.size() - 7;
			if (expmaIndex < 0)
			{
				PrintLog("信息不全，无法预测");
				result += " 无法预测";
				break;
			}
			double lastExpma = *(data.expmaDataVector.rbegin());
			double lastClosePrice = data.dataVector.rbegin()->closePrice;
			double crucialClose = (7 * lastExpma - data.dataVector[expmaIndex].closePrice) / 6;
			double lowerBound = max(crucialLow, crucialClose);
			double fellPercentage = (lowerBound - lastClosePrice) / lastClosePrice;
			double profit = (lowerBound - buyInPrice) / buyInPrice;
			sprintf_s(tempString, "下一交易日卖出信号：%.2lf，跌幅：%.2lf%%，获利：%.2lf%%", lowerBound, fellPercentage * 100, profit * 100);
			PrintLog(tempString);
			result += (std::string(" ") + tempString);
		}
		else //当前空仓，计算买入信号
		{
			size_t expmaIndex = data.dataVector.size() - 8;
			if (expmaIndex < 0)
			{
				PrintLog("信息不全，无法预测");
				result += " 无法预测";
				break;
			}
			double lastExpma = *(data.expmaDataVector.rbegin());
			double lastClosePrice = data.dataVector.rbegin()->closePrice;
			double crucialClose1 = (7 * lastExpma - data.dataVector[expmaIndex + 1].closePrice) / 6;
			double crucialClose2 = data.dataVector.rbegin()->closePrice + data.dataVector[expmaIndex + 1].closePrice - data.dataVector[expmaIndex].closePrice;
			double upperBound = max(crucialClose1, crucialClose2);
			upperBound = max(upperBound, crucialHigh);
			double flyPercentage = (upperBound - lastClosePrice) / lastClosePrice;
			sprintf_s(tempString, "下一交易日买入信号：%.2lf，涨幅：%.2lf%%", upperBound, flyPercentage * 100);
			PrintLog(tempString);
			result += (std::string(" ") + tempString);
		}
	} while (false);
	return result;
}
