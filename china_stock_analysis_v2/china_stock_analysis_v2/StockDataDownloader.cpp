#include "StdAfx.h"
#include "StockDataDownloader.h"
#include "StockDataLoader.h"
#include "utility.h"
#include "micro.h"
#include <stdlib.h>

using namespace std;

CStockDataDownloader::CStockDataDownloader(void)
	: mCurl(NULL), mErrorCode(CURLE_FAILED_INIT), mResponseErrCode(-1)
{
	mBaseDir = CacheDataDir;
	mCurl = curl_easy_init();
	if (mCurl == NULL)
		return;
	if (!SetOption(CURLOPT_WRITEFUNCTION, data_callback))
		return;
	if (!SetOption(CURLOPT_WRITEDATA, &mBuffer))
		return;
	if (!SetOption(CURLOPT_NOSIGNAL, 1))
		return;
	if (!SetOption(CURLOPT_TIMEOUT, 10))
		return;

	mErrorCode = CURLE_OK;
}

CStockDataDownloader::~CStockDataDownloader(void)
{
	if (mCurl)
	{
		curl_easy_cleanup(mCurl);
		mCurl = NULL;
	}
}

size_t CStockDataDownloader::data_callback(char *data, size_t size, size_t nmemb, void * mp)
{
	uint32_t totalSize = size * nmemb;
	string& buffer = *((string* )mp);
	buffer.append(data, totalSize);
	return totalSize;
}

bool CStockDataDownloader::Download(const std::string& stockID,
	const DateType& beginDate,
	const DateType& endDate,
	const std::string& fileName/* = ""*/)
{
	string url = GenerateUrl(stockID, beginDate, endDate);
	string realFileName(fileName);
	if (realFileName.empty())
	{
		realFileName = mBaseDir + stockID + ".csv";
	}
	bool ret = DownloadURL(url); //not write to file, we need to write table title first
	if (ret)
	{
		std::string outputString = "Date,Open,High,Close,Low,Volume\n";
		outputString += mBuffer;
		WriteToFile(realFileName, outputString);
	}
	return ret;
}

bool CStockDataDownloader::DownloadURL(const std::string& url)
{
	bool ret(false);
	if(mCurl)
	{
		try
		{
			ret = Get(url.c_str());
			ret = (mResponseErrCode == 200);
		}
		catch (...)
		{}
	}
	return ret;
}

bool CStockDataDownloader::SetUrl(const char* url)
{
	return SetOption(CURLOPT_URL, url);
}

bool CStockDataDownloader::Perform()
{
	mBuffer.clear();
	mResponseErrCode = -1;
	CURLcode ret = curl_easy_perform(mCurl);
	mErrorCode = ret;
	if(mErrorCode == CURLE_OK)
	{
		curl_easy_getinfo(mCurl, CURLINFO_RESPONSE_CODE , &mResponseErrCode);
	}
	return (mErrorCode == CURLE_OK);
}

std::string CStockDataDownloader::GenerateUrl( const std::string& stockID, const DateType& beginDate, const DateType& endDate)
{
	string url;
	DEFINE_TEMP_BUFFER(buffer);
	url = BaslUrlOfSinaSource;
	std::string stockIDOfSina;
	if (stockID[0] == '6') //上海股票以‘6’开头 
	{
		stockIDOfSina = "sh";
	} else
	{
		stockIDOfSina = "sz";
	}
	stockIDOfSina += stockID;
	sprintf_s(buffer, TEMP_BUFFER_SIZE, "symbol=%s&end_date=%s&begin_date=%s&type=plain",
		stockIDOfSina.c_str(), endDate.ToString().c_str(), beginDate.ToString().c_str());

	url += buffer;
	return url;
}