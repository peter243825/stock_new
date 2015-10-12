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
	PrintLog(std::string("��Ʊ���룺") + stockID + "��ģ���·ݣ�" + selectedMonth);

	//�����������䣨��ʼʱ��Ϊģ���·ݼ�2��1�ţ�����ʱ��Ϊģ���·ݵ����һ�죩
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
		PrintLog(std::string("�������ݣ�") + beginDate.ToString() + " -> " + endDate.ToString());
		CStockDataDownloader downloader;
		bool ret = downloader.Download(stockID, beginDate, endDate);
		if (!ret)
		{
			PrintLog("��������ʧ��");
			result += "��������ʧ��";
			break;
		}
		PrintLog("��ȡ����");
		std::string fileName = CacheDataDir + stockID + ".csv";
		CStockDataLoader loader;
		size_t size = loader.LoadFromFile(fileName, true);
		if (size <= 0)
		{
			PrintLog("��ȡ����ʧ��");
			result += "��ȡ����ʧ��";
			break;
		}
		StockData data = loader.GetStockData();
		sprintf_s(tempString, "��������%d", size);
		PrintLog(tempString);
		PrintLog("��ʼģ��");
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
			PrintLog("û���ҵ�����K������");
			result += "û���ҵ�����K������";
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
				lastMonthState = "������";
			}
			else
			{
				lastMonthState = "���ƾ�";
			}
		}
		else
		{
			if (deltaLow < deltaHalf)
			{
				lastMonthState = "���ؾ�";
			}
			else
			{
				lastMonthState = "���ƾ�";
			}
		}
		sprintf_s(tempString, "����K���֣̾���� %.2lf����� %.2lf�������̼� %.2lf��%s", highPriceInMonth, lowPriceInMonth, closePriceInMonth, lastMonthState.c_str());
		PrintLog(tempString);
		result += lastMonthState;
		if (iter == endIter)
		{
			PrintLog("û���ҵ����µ�K������");
			result += " û���ҵ����µ�K������";
			break;
		}
		double crucialHigh, crucialLow;
		if (lastMonthState == "���ؾ�")
		{
			result += " ���ؾֲ�����";
			PrintLog("���ؾֲ�����");
			break;
		}
		else if (lastMonthState == "������")
		{
			crucialHigh = highPriceInMonth;
			crucialLow = iter->lowPrice;
		}
		else
		{
			crucialHigh = iter->highPrice;
			crucialLow = iter->lowPrice;
		}
		sprintf_s(tempString, "���µĹؼ��������� %.2lf������ %.2lf", crucialLow, crucialHigh);
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
			//�����źţ��ɼ����ǣ�7�վ������������7�վ���֮�ϣ������ڹؼ�����֮��
			if (buyInPrice < 0 && iter->closePrice > iter->openPrice && data.expmaDataVector[expmaIndex] > predictExpma && iter->closePrice > data.expmaDataVector[expmaIndex] && iter->closePrice > crucialHigh)
			{
				buyInPrice = (iter->closePrice + iter->openPrice) / 2;
				sprintf_s(tempString, "%s���������źţ�����ۣ�%.2lf", iter->date.ToString().c_str(), buyInPrice);
				PrintLog(tempString);
				result += (std::string(" ") + tempString);
			}
			//�����źţ�������7�վ���֮�£���ɼ۵��ƹؼ�����
			if (buyInPrice > 0 && (iter->closePrice < predictExpma || iter->closePrice < crucialLow))
			{
				double sellOutPrice = (iter->closePrice + iter->openPrice) / 2;
				sprintf_s(tempString, "%s���������źţ������ۣ�%.2lf", iter->date.ToString().c_str(), sellOutPrice);
				PrintLog(tempString);
				result += (std::string(" ") + tempString);
				profit += (sellOutPrice - buyInPrice) / buyInPrice;
				buyInPrice = -1;
			}
		}
		if (buyInPrice > 0) //���ڳֲ�״̬
		{
			profit += (data.dataVector.rbegin()->closePrice - buyInPrice) / buyInPrice;
		}
		sprintf_s(tempString, "ģ�����������������Ϊ��%.2lf%%", profit * 100);
		PrintLog(tempString);
		if (DOUBLE_IS_ZERO(profit))
		{
			result += " �����޲���";
		}
		else
		{
			sprintf_s(tempString, " ����������Ϊ��%.2lf%%", profit * 100);
			result += tempString;
		}
	} while (false);
	return result;
}

std::string CAnalysisEngine::SingleStockPredict(const std::string& stockID)
{
	DEFINE_TEMP_BUFFER(tempString);
	std::string result;
	PrintLog(std::string("��Ʊ���룺") + stockID);

	//�����������䣨��ʼʱ��Ϊ��ǰ�·ݼ�2��1�ţ�����ʱ��Ϊ���죩
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
		PrintLog(std::string("�������ݣ�") + beginDate.ToString() + " -> " + endDate.ToString());
		CStockDataDownloader downloader;
		bool ret = downloader.Download(stockID, beginDate, endDate);
		if (!ret)
		{
			PrintLog("��������ʧ��");
			result += "��������ʧ��";
			break;
		}
		PrintLog("��ȡ����");
		std::string fileName = CacheDataDir + stockID + ".csv";
		CStockDataLoader loader;
		size_t size = loader.LoadFromFile(fileName, true);
		if (size <= 0)
		{
			PrintLog("��ȡ����ʧ��");
			result += "��ȡ����ʧ��";
			break;
		}
		StockData data = loader.GetStockData();
		sprintf_s(tempString, "��������%d", size);
		PrintLog(tempString);
		PrintLog("��ʼԤ��");
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
			PrintLog("û���ҵ�����K������");
			result += "û���ҵ�����K������";
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
				lastMonthState = "������";
			}
			else
			{
				lastMonthState = "���ƾ�";
			}
		}
		else
		{
			if (deltaLow < deltaHalf)
			{
				lastMonthState = "���ؾ�";
			}
			else
			{
				lastMonthState = "���ƾ�";
			}
		}
		sprintf_s(tempString, "����K���֣̾���� %.2lf����� %.2lf�������̼� %.2lf��%s", highPriceInMonth, lowPriceInMonth, closePriceInMonth, lastMonthState.c_str());
		PrintLog(tempString);
		result += lastMonthState;
		if (iter == endIter)
		{
			PrintLog("û���ҵ����µ�K������");
			result += " û���ҵ����µ�K������";
			break;
		}
		double crucialHigh, crucialLow;
		if (lastMonthState == "���ؾ�")
		{
			result += " ���ؾֲ�����";
			PrintLog("���ؾֲ�����");
			break;
		}
		else if (lastMonthState == "������")
		{
			crucialHigh = highPriceInMonth;
			crucialLow = iter->lowPrice;
		}
		else
		{
			crucialHigh = iter->highPrice;
			crucialLow = iter->lowPrice;
		}
		sprintf_s(tempString, "���µĹؼ��������� %.2lf������ %.2lf", crucialLow, crucialHigh);
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
			//�����źţ��ɼ����ǣ�7�վ������������7�վ���֮�ϣ������ڹؼ�����֮��
			if (buyInPrice < 0 && iter->closePrice > iter->openPrice && data.expmaDataVector[expmaIndex] > predictExpma && iter->closePrice > data.expmaDataVector[expmaIndex] && iter->closePrice > crucialHigh)
			{
				buyInPrice = (iter->closePrice + iter->openPrice) / 2;
				sprintf_s(tempString, "%s���������źţ�����ۣ�%.2lf", iter->date.ToString().c_str(), buyInPrice);
				PrintLog(tempString);
				result += (std::string(" ") + tempString);
			}
			//�����źţ�������7�վ���֮�£���ɼ۵��ƹؼ�����
			if (buyInPrice > 0 && (iter->closePrice < predictExpma || iter->closePrice < crucialLow))
			{
				double sellOutPrice = (iter->closePrice + iter->openPrice) / 2;
				sprintf_s(tempString, "%s���������źţ������ۣ�%.2lf", iter->date.ToString().c_str(), sellOutPrice);
				PrintLog(tempString);
				result += (std::string(" ") + tempString);
				buyInPrice = -1;
			}
		}
		if (buyInPrice > 0) //��ǰ�ֲ֣����������ź�
		{
			size_t expmaIndex = data.dataVector.size() - 7;
			if (expmaIndex < 0)
			{
				PrintLog("��Ϣ��ȫ���޷�Ԥ��");
				result += " �޷�Ԥ��";
				break;
			}
			double lastExpma = *(data.expmaDataVector.rbegin());
			double lastClosePrice = data.dataVector.rbegin()->closePrice;
			double crucialClose = (7 * lastExpma - data.dataVector[expmaIndex].closePrice) / 6;
			double lowerBound = max(crucialLow, crucialClose);
			double fellPercentage = (lowerBound - lastClosePrice) / lastClosePrice;
			double profit = (lowerBound - buyInPrice) / buyInPrice;
			sprintf_s(tempString, "��һ�����������źţ�%.2lf��������%.2lf%%��������%.2lf%%", lowerBound, fellPercentage * 100, profit * 100);
			PrintLog(tempString);
			result += (std::string(" ") + tempString);
		}
		else //��ǰ�ղ֣����������ź�
		{
			size_t expmaIndex = data.dataVector.size() - 8;
			if (expmaIndex < 0)
			{
				PrintLog("��Ϣ��ȫ���޷�Ԥ��");
				result += " �޷�Ԥ��";
				break;
			}
			double lastExpma = *(data.expmaDataVector.rbegin());
			double lastClosePrice = data.dataVector.rbegin()->closePrice;
			double crucialClose1 = (7 * lastExpma - data.dataVector[expmaIndex + 1].closePrice) / 6;
			double crucialClose2 = data.dataVector.rbegin()->closePrice + data.dataVector[expmaIndex + 1].closePrice - data.dataVector[expmaIndex].closePrice;
			double upperBound = max(crucialClose1, crucialClose2);
			upperBound = max(upperBound, crucialHigh);
			double flyPercentage = (upperBound - lastClosePrice) / lastClosePrice;
			sprintf_s(tempString, "��һ�����������źţ�%.2lf���Ƿ���%.2lf%%", upperBound, flyPercentage * 100);
			PrintLog(tempString);
			result += (std::string(" ") + tempString);
		}
	} while (false);
	return result;
}
