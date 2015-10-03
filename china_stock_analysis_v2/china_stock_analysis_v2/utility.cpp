#include "stdafx.h"

#include "utility.h"
#include <algorithm>
#include <stdlib.h>
#include <stdio.h>
#include <time.h>
#include <math.h>
using namespace std;

string TcharToString(const TCHAR * tcharString)
{
	if (tcharString == NULL)
	{
		return string("");
	}
	DWORD dwNum = WideCharToMultiByte(CP_OEMCP, NULL, tcharString, -1,NULL, 0, NULL, FALSE);
	char *buf = new char[dwNum];
	if(!buf)
	{
		delete []buf;
		AfxMessageBox(CString("没有足够的内存进行字符串转换！"));
		return string("");
	}
	WideCharToMultiByte(CP_OEMCP, NULL, tcharString, -1, buf, dwNum, NULL, FALSE);
	string tempString(buf);
	delete []buf;
	return tempString;
}

CString StringToTchar(const char *str)
{
	if (str == NULL)
	{
		return CString("");
	}
	DWORD dwNum = MultiByteToWideChar(CP_ACP, NULL, str, -1, NULL, 0);
	TCHAR *buf = new TCHAR[dwNum];
	if(!buf)
	{
		delete []buf;
		AfxMessageBox(CString("没有足够的内存进行字符串转换！"));
		return CString("");
	}
	MultiByteToWideChar(CP_ACP, NULL, str, -1, buf, dwNum);
	CString tempString(buf);
	delete []buf;
	return tempString;
}

CString StringToTchar(const std::string& str)
{
	if (str.empty())
	{
		return CString("");
	}
	else
	{
		return StringToTchar(str.c_str());
	}
}

std::vector<std::string> StringSplitter(const std::string& str, const std::string& splitter)
{
	vector<string> splitResult;
	size_t pos;
	size_t lastPos(0);
	for (pos = str.find(splitter); pos != string::npos; pos = str.find(splitter, lastPos))
	{
		splitResult.push_back(str.substr(lastPos, pos - lastPos));
		lastPos = pos + splitter.size();
	}
	if (lastPos < str.length())
	{
		splitResult.push_back(str.substr(lastPos));
	}
	return splitResult;
}

int32_t StringToInteger(const std::string& numStr, std::string& remainStr/* = std::string()*/)
{
	int32_t ret(0);
	bool isNegative(false);
	size_t pos(0);
	remainStr.clear();
	for (; pos < numStr.length(); ++pos)
	{
		char ch = numStr[pos];
		if (isdigit(ch) || ch == '-' || ch == '+')
		{
			break;
		}
	}
	if (numStr.length() == pos)
	{
		return ret;
	}

	if (numStr[pos] == '-' || numStr[pos] == '+')
	{
		isNegative = (numStr[pos] == '-');
		++pos;
	}
	for (; pos < numStr.length(); ++pos)
	{
		char ch = numStr[pos];
		if (isdigit(ch))
		{
			ret = ret * 10 + (ch - '0');
		}
		else
		{
			break;
		}
	}
	remainStr = numStr.substr(pos);
	return isNegative ? (-1 * ret) : (ret);
}

double StringToDouble(const std::string& numStr, std::string& remainStr/* = std::string()*/)
{
	double ret(0.0);
	remainStr.clear();
	ret = StringToInteger(numStr, remainStr);
	if (remainStr.empty() || remainStr[0] != '.')
	{
		return ret;
	}
	bool isNegative = (ret < 0);
	if (isNegative)
	{
		ret *= -1;
	}
	double divideBase(10);
	size_t pos(1);
	for (; pos < remainStr.length(); ++pos)
	{
		char ch = remainStr[pos];
		if (isdigit(ch))
		{
			ret += (ch - '0') / divideBase;
			divideBase *= 10;
		}
		else
		{
			break;
		}
	}
	remainStr = remainStr.substr(pos);
	return isNegative ? (-1 * ret) : (ret);
}

std::string BoolToString(bool boolVal)
{
	if (boolVal)
	{
		return string("true");
	}
	else
	{
		return string("false");
	}
}

bool StringToBool(const std::string& boolStr)
{
	if (boolStr == "true")
	{
		return true;
	}
	else
	{
		return false;
	}
}

int32_t WriteToFile(const std::string& fileName, const std::string& context, bool isAppend/* = false*/)
{
	FILE *outfile;
	int32_t ret;
	if (isAppend)
	{
		ret = fopen_s(&outfile, fileName.c_str(), "a");
	} 
	else
	{
		ret = fopen_s(&outfile, fileName.c_str(), "w");
	}
	if (ret != 0)
	{
		return ret;
	}
	
	ret = fprintf_s(outfile, "%s", context.c_str());
	fclose(outfile);
	return ret;
}

int32_t ReadFromFile(const std::string& fileName, std::string& context)
{
	FILE *infile;
	int32_t ret;
	char buffer[1024];
	context.clear();
	ret = fopen_s(&infile, fileName.c_str(), "r");
	if (ret != 0)
	{
		return ret;
	}
	while (fgets(buffer, 1024, infile))
	{
		context += buffer;
	}
	fclose(infile);
	return (int32_t) context.size();
}
