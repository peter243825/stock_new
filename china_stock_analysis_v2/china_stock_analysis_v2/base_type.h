#ifndef BASE_TYPE_H
#define BASE_TYPE_H
#include "utility.h"
#include "micro.h"
#include <stdint.h>
#include <string>
#include <stdlib.h>

//���ڽṹ
struct DateType
{
	int32_t year;
	int32_t month;
	int32_t date;
	DateType(int32_t _year = 0, int32_t _month = 0, int32_t _date = 0)
		: year(_year), month(_month), date(_date)
	{}

	DateType(const std::string& dataStr)
		: year(0), month(0), date(0)
	//dataStr is like: "2014-03-15"
	{
		FromString(dataStr);
	}

	std::string ToString() const
	{
		char buffer[1000];
		sprintf_s(buffer, 1000, "%d-%d-%d", year, month, date);
		return std::string(buffer);
	}

	void FromString(const std::string& dataStr)
	//dataStr is like: "2014-03-15"
	{
		std::vector<std::string> splitResult = StringSplitter(dataStr, "-");
		if (splitResult.size() == 3)
		{
			year = StringToInteger(splitResult[0]);
			month = StringToInteger(splitResult[1]);
			date = StringToInteger(splitResult[2]);
		}
		else
		{
			//do nothing
		}
	}

	void Adjust()
	{
		int32_t dayCount[] = {31, 31, 28, 31, 30,
								  31, 30, 31, 31,
								  30, 31, 30, 31, 31};//dayCount[0]������һ���12�£�dayCount[13]������һ���1��
		//correct year & month firstly
		while(month <= 0)
		{
			month += 12;
			year--;
		}
		while (month > 12)
		{
			month -= 12;
			year++;
		}
		if (year < 0)
		{
			year = 0;
		}
		while (date <= 0 || date > dayCount[month])
		{
			//��������
			if (year % 4 != 0 || (year % 100 == 0 && year % 400 != 0))
			{
				dayCount[2] = 28;
			}
			else
			{
				dayCount[2] = 29;
			}
			//adjust date
			if (date <= 0)
			{
				--month;
				date += dayCount[month];
			}
			else if (date > dayCount[month])
			{
				date -= dayCount[month];
				++month;
			}
			//adjust month
			if (month <= 0)
			{
				--year;
				month = 12;
			}
			else if (month > 12)
			{
				++year;
				month = 1;
			}
		}
	}
	DateType& operator+=(int32_t delta)
	{
		//������ݽṹ��δset�õģ�����delta = 0�������κβ���
		if (delta == 0 || (*this).date <= 0 || (*this).month <= 0 || (*this).year <= 0)
		{
			return *this;
		}

		(*this).date += delta;
		(*this).Adjust();
		return *this;
	}
	DateType& operator-=(int32_t delta)
	{
		(*this).operator +=(-1 * delta);
		return *this;
	}

	static DateType GetDate(int32_t dayGap) //dayGap����0��ȡδ��ʱ�䣬С��0��ȡ��ʷʱ�䣬����0��ȡ����ʱ��
	{
		SYSTEMTIME time;
		GetLocalTime(&time);
		DateType date(time.wYear, time.wMonth, time.wDay);
		date += dayGap;
		return date;
	}

	static bool LaterThan(const DateType& date1, const DateType& date2) //return true if date1 is after date2: LaterThan(2013-12-10, 2013-12-9) == true
	{
		if (date1.year > date2.year ||
			(date1.year == date2.year && date1.month > date2.month) ||
			(date1.year == date2.year && date1.month == date2.month && date1.date > date2.date))
		{
			return true;
		}
		else
		{
			return false;
		}
	}

	static bool IsSameMonth(const DateType& date1, const DateType& date2) //return true if date1's year & month is the same as date2's: IsSameMonth(2013-12-15, 2013-12-9) == true
	{
		if (date1.year == date2.year && date1.month == date2.month)
		{
			return true;
		}
		else
		{
			return false;
		}
	}

	friend bool operator > (const DateType& date1, const DateType& date2) //return true if date1 is after date2: (2013-12-10 > 2013-12-9) == true
	{
		if (date1.year > date2.year ||
			(date1.year == date2.year && date1.month > date2.month) ||
			(date1.year == date2.year && date1.month == date2.month && date1.date > date2.date))
		{
			return true;
		}
		else
		{
			return false;
		}
	}

	friend bool operator == (const DateType& date1, const DateType& date2) //return true if date1 is equal to date2: (2013-12-10 == 2013-12-10) == true
	{
		if (date1.year == date2.year && date1.month == date2.month && date1.date == date2.date)
		{
			return true;
		}
		else
		{
			return false;
		}
	}

	friend bool operator >=(const DateType& date1, const DateType& date2) //return true if date1 is not before date2: (2013-12-10 >= 2013-12-9) == true, (2013-12-10 >= 2013-12-10) == true
	{
		if (date1 > date2 || date1 == date2)
		{
			return true;
		}
		else
		{
			return false;
		}
	}
};

//��Ʊ���ݽṹ
struct StockDataItem
{
	DateType date;
	double openPrice;
	double highPrice;
	double lowPrice;
	double closePrice;
	int32_t volume;
	double adjClosePrice;

	StockDataItem()
		: openPrice(0.0), highPrice(0.0), lowPrice(0.0), closePrice(0.0), volume(0), adjClosePrice(0.0)
	{}

	StockDataItem(const std::vector<std::string>& rowTitle, const std::string& dataStr)
		: openPrice(0.0), highPrice(0.0), lowPrice(0.0), closePrice(0.0), volume(0), adjClosePrice(0.0)
	//rowTitle gets from "Date,Open,High,Low,Close,Volume,Adj Close"
	//dataStr is like: "2014-03-15,8.47,8.67,8.39,8.56,110468600,8.56"
	{
		FromString(rowTitle, dataStr);
	}

	void FromString(const std::vector<std::string>& rowTitle, const std::string& dataStr)
	//rowTitle gets from "Date,Open,High,Low,Close,Volume,Adj Close"
	//dataStr is like: "2014-03-15,8.47,8.67,8.39,8.56,110468600,8.56"
	{
		std::vector<std::string> splitResult = StringSplitter(dataStr, ",");
		const size_t rowCount(splitResult.size());
		if (rowCount == rowTitle.size())
		{
			size_t i;
			for (i = 0; i < rowCount; ++i)
			{
				const std::string& numStr(splitResult[i]);
				if (rowTitle[i] == "Date")
				{
					date.FromString(numStr);
				}
				else if (rowTitle[i] == "Open")
				{
					openPrice = StringToDouble(numStr);
				}
				else if (rowTitle[i] == "High")
				{
					highPrice = StringToDouble(numStr);
				}
				else if (rowTitle[i] == "Low")
				{
					lowPrice = StringToDouble(numStr);
				}
				else if (rowTitle[i] == "Close")
				{
					closePrice = StringToDouble(numStr);
				}
				else if (rowTitle[i] == "Volume")
				{
					volume = StringToInteger(numStr);
				}
				else if (rowTitle[i] == "Adj Close")
				{
					adjClosePrice = StringToDouble(numStr);
				}
				else
				{
					//skip
				}
			}			
		}
		else
		{
			//do nothing
		}
	}

	bool IsNoTradeData()
	{
		return (volume == 0);
	}

	static bool LaterThan(const StockDataItem& item1, const StockDataItem& item2) //return true if item1's date is after item2's date
	{
		return DateType::LaterThan(item1.date, item2.date);
	}
};

//���������ݽṹ
//�����ߵ�����ʵ��Ϊn�����̼۵�ƽ��ֵ
//�Ϲ���Ϊ����+2*��׼��
//�¹���Ϊ����-2*��׼��
struct BolingerBandsData
{
	double mid;
	double up;
	double down;
	int32_t dayCount;

	BolingerBandsData(double _mid = 0.0, double _up = 0.0, double _down = 0.0, int32_t _dayCount = 0)
		: mid(_mid), up(_up), down(_down), dayCount(_dayCount)
	{}

	static BolingerBandsData GenerateFromStandardDeviationData(double meanValue, double standardDeviation, int32_t dayCount)
	{
		double mid = meanValue;
		double up = mid + 2 * standardDeviation;
		double down = mid - 2 * standardDeviation;
		return BolingerBandsData(mid, up, down, dayCount);
	}
};

//MACD�����ݽṹ
//DIF�ߣ���12��EMA��ֵ��ȥ26��EMA��ֵ��EMAΪָ��ƽ����ָ�꣩
//12��EMA�ļ��㣺EMA12 = ǰһ��EMA12 * 11/13 + �������� * 2/13
//26��EMA�ļ��㣺EMA26 = ǰһ��EMA26 * 25/27 + �������� * 2/27
//DIF = EMA12 - EMA26
//DEA = ��ǰһ��DEA * 8/10 + ����DIF * 2/10��
//MACD��״ͼ = DIF - DEA
struct MACDData 
{
	double EMA12;
	double EMA26;
	double DIF;
	double DEA;
	MACDData(double _EMA12 = 0.0, double _EMA26 = 0.0, double _DIF = 0.0, double _DEA = 0.0)
		: EMA12(_EMA12), EMA26(_EMA26), DIF(_DIF), DEA(_DEA)
	{}

	double GetMACD()
	{
		return DIF - DEA;
	}

	static MACDData GenerateData(const MACDData& lastDayData, const StockDataItem& todayData)
	{
		double ema12 = (lastDayData.EMA12 * 11 + todayData.closePrice * 2) / 13;
		double ema26 = (lastDayData.EMA26 * 25 + todayData.closePrice * 2) / 27;
		double dif = ema12 - ema26;
		double dea = (lastDayData.DEA * 8 + dif * 2) / 10;
		return MACDData(ema12, ema26, dif, dea);
	}
};

//EXPMA�����ݽṹ
//EXPMA��ʵ������n�����̼۵ľ���
typedef double EXPMAData;

struct StockData
{
	std::string stockID;
	std::vector<StockDataItem> dataVector;
	std::vector<BolingerBandsData> bollDataVector;
	std::vector<MACDData> macdDataVector;
	std::vector<EXPMAData> expmaDataVector;
	std::vector<double> meanVolumeVector; //�ɽ�����ֵ

	void GenerateBollDataVector(int32_t dayCount)
	{
		bollDataVector.clear();
		if (dataVector.size() < (size_t)dayCount)
		{
			return;
		}
		std::vector<StockDataItem>::iterator beginIter(dataVector.begin()), endIter(dataVector.begin() + dayCount - 1); //ѭ������
		StockDataItem item;
		size_t offset = (char *)(&(item.closePrice)) - (char *)(&item);//��ͳ�Ʊ���Ϊdouble closePrice
		//��ʼͳ��
		for (;endIter != dataVector.end(); ++beginIter, ++endIter)
		{
			double meanValue = CalculateMeanValueFromDataVector<std::vector<StockDataItem>::iterator, double>
								(beginIter, endIter, offset);
			double standardDeviation = CalculateStandardDeviationFromDataVector<std::vector<StockDataItem>::iterator, double>
										(beginIter, endIter, meanValue, offset);
			bollDataVector.push_back(BolingerBandsData::GenerateFromStandardDeviationData(meanValue, standardDeviation, dayCount));
		}
	}

	void GenerateMACDDataVector()
	{
		macdDataVector.clear();
		if (dataVector.size() < 1)
		{
			return;
		}
		int32_t i = (int32_t) dataVector.size() - 1, count = 0;
		MACDData data(dataVector[i].closePrice, dataVector[i].closePrice, 0, 0);
		std::vector<MACDData> tmpDataVector; //������Ҫ�������ɣ�����ȷ���һ����ʱ������
		tmpDataVector.push_back(data);

		//��ʼͳ��
		for (--i; i >= 0; --i, ++count)
		{
			tmpDataVector.push_back(MACDData::GenerateData(tmpDataVector[count], dataVector[i]));
		}
		//�����ݵ������macdDataVector
		for (i = (int32_t)tmpDataVector.size() - 1; i >= 0; --i)
		{
			macdDataVector.push_back(tmpDataVector[i]);
		}
	}

	void GenerateEXPMADataVector(int32_t dayCount)
	{
		expmaDataVector.clear();
		if (dataVector.size() < (size_t)dayCount)
		{
			return;
		}
		std::vector<StockDataItem>::iterator beginIter(dataVector.begin()), endIter(dataVector.begin() + dayCount - 1); //ѭ������
		StockDataItem item;
		size_t offset = (char *)(&(item.closePrice)) - (char *)(&item);//��ͳ�Ʊ���Ϊdouble closePrice
		//��ʼͳ��
		for (;endIter != dataVector.end(); ++beginIter, ++endIter)
		{
			double meanValue = CalculateMeanValueFromDataVector<std::vector<StockDataItem>::iterator, double>
				(beginIter, endIter, offset);
			expmaDataVector.push_back(meanValue);
		}
	}

	void GenerateMeanVolumeVector(int32_t dayCount)
	{
		meanVolumeVector.clear();
		if (dataVector.size() < (size_t)dayCount)
		{
			return;
		}
		std::vector<StockDataItem>::iterator beginIter(dataVector.begin()), endIter(dataVector.begin() + dayCount - 1); //ѭ������
		StockDataItem item;
		size_t offset = (char *)(&(item.volume)) - (char *)(&item);//��ͳ�Ʊ���Ϊint32_t volume
		//��ʼͳ��
		for (;endIter != dataVector.end(); ++beginIter, ++endIter)
		{
			double meanValue = CalculateMeanValueFromDataVector<std::vector<StockDataItem>::iterator, int32_t>
				(beginIter, endIter, offset);
			meanVolumeVector.push_back(meanValue);
		}
	}

	//������һ�����ݵ�RSIֵ
	//RSI���㹫ʽ��
	//LC��=REF��CLOSE,1���� 
	//RSI1��SMA��MAX��CLOSE-LC,0��,N1,1��/SMA��ABS��CLOSE-LC��,N1,1��*100��
	//SMAΪ��Ȩƽ����������LCΪ��һ�����̼�
	double GetLastDayRSI(int32_t dayCount)
	{
		if (dataVector.size() < (size_t)dayCount + 1)
		{
			return 0.0;
		}
		int32_t preDayCount = 0;
		const double baseFactor = (dayCount - 1) / (double)(dayCount);
		/*
		double currentFactor = 1.0;
		while (currentFactor > 0.001)
		{
			++preDayCount;
			currentFactor *= baseFactor;
		}
		int32_t beginDay = min(dataVector.size() - 1, (size_t)preDayCount); //��Ϊʹ�ü�Ȩƽ���������Ҫ��preDayCount��֮ǰ�����ݿ�ʼ����
		*/
		int32_t beginDay = dataVector.size() - 1; //Ϊ��ʹRSI���ݸ���׼ȷ������������ݿ�ʼ����
		int32_t i;
		double incSum = 0, totalSum = 0;
		StockDataItem dataItemLastDay = dataVector[beginDay];
		for (i = beginDay - 1; i >= 0; --i)
		{
			incSum *= baseFactor;
			totalSum *= baseFactor;
			StockDataItem dataItemToday = dataVector[i];
			double priceDelta = dataItemToday.closePrice - dataItemLastDay.closePrice;
			incSum += max(0, priceDelta) / (dayCount);
			totalSum += abs(priceDelta) / (dayCount);
			dataItemLastDay = dataItemToday;
		}
		double rsi = 100 * incSum / totalSum;
		return rsi;
	}
};

struct StockListItem
{
	std::string stockID;
	std::string stockName;
	double publishPrice;
	bool isUpdated;
	StockListItem()
		: publishPrice(0.0), isUpdated(false)
	{}

	StockListItem(const std::vector<std::string>& rowTitle, const std::string& dataStr)
		: publishPrice(0.0), isUpdated(false)
		//rowTitle gets from "Stock ID,Stock Name,Publish Price"
		//dataStr is like: "600000.SS,�ַ�����,8.67"
	{
		FromString(rowTitle, dataStr);
	}

	std::string ToString()
	{
		DEFINE_TEMP_BUFFER(buffer);
		sprintf_s(buffer, TEMP_BUFFER_SIZE, "%s,%s,%.2lf", stockID.c_str(), stockName.c_str(), publishPrice);
		return std::string(buffer);
	}

	void FromString(const std::vector<std::string>& rowTitle, const std::string& dataStr)
		//rowTitle gets from "Stock ID,Stock Name,Publish Price"
		//dataStr is like: "600000.SS,�ַ�����,8.67"
	{
		std::vector<std::string> splitResult = StringSplitter(dataStr, ",");
		const size_t rowCount(splitResult.size());
		if (rowCount == rowTitle.size())
		{
			size_t i;
			for (i = 0; i < rowCount; ++i)
			{
				const std::string& tempStr(splitResult[i]);
				if (rowTitle[i] == "Stock ID")
				{
					stockID = tempStr;
				}
				else if (rowTitle[i] == "Stock Name")
				{
					stockName = tempStr;
				}
				else if (rowTitle[i] == "Publish Price")
				{
					publishPrice = StringToDouble(tempStr);
				}
				else
				{
					//skip
				}
			}			
		}
		else
		{
			//do nothing
		}
	}
	bool operator < (const StockListItem& item1) const
	{
		return ((*this).stockID < item1.stockID);
	}
};

typedef std::vector<StockData> StockVector;
typedef std::vector<StockListItem> StockList;

#endif //BASE_TYPE_H