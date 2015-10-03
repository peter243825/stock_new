#pragma once
#include "curl/curl.h"
#include "base_type.h"

class CStockDataDownloader
{
public:
	CStockDataDownloader(void);

	static size_t data_callback(char *ptr, size_t size, size_t nmemb, void * mp);

	bool Download(const std::string& stockID,
						  const DateType& beginDate,
						  const DateType& endDate,
						  const std::string& fileName = ""); //interface to download From sina source

	bool DownloadURL(const std::string& url);

	bool Get(const char* url)
	{
		if (!SetOption(CURLOPT_HTTPGET, 1))
			return false;
		if (!SetUrl(url))
			return false;
		return Perform();
	}

	bool SetUrl(const char* url);

	bool Perform();

	const std::string& GetResponse() const
	{
		return mBuffer;
	}

	~CStockDataDownloader(void);

private:
	template <typename T>
	bool SetOption(CURLoption option, T parameter)
	{
		mErrorCode = curl_easy_setopt(mCurl, option, parameter);
		return mErrorCode == CURLE_OK;
	}

	std::string GenerateUrl(const std::string& stockID, const DateType& beginDate, const DateType& endDate);

	std::string mBuffer;
	CURL* mCurl;
	CURLcode mErrorCode;
	int32_t mResponseErrCode;
	std::string mBaseDir;
};

