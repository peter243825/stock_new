#ifndef UTILITY_H
#define UTILITY_H
#include <string>
#include <vector>
#include <stdint.h>

/*
 *  TcharToString：将一个TCHAR字符串转化为std::string字符串
 *  @tcharString：一个TCHAR字符串，输入参数
 *  @return：与输入字符串相对应的std::string字符串
 */
std::string TcharToString(const TCHAR * tcharString);

/*
 *  StringToTchar：将一个char*字符串转化为CString字符串
 *  @str：一个char*字符串，输入参数
 *  @return：与输入字符串相对应的CString字符串
 */
CString StringToTchar(const char *str);
/*
 *  StringToTchar：std::string输入版
 *  @str：一个std::string字符串，输入参数
 *  @return：与输入字符串相对应的CString字符串
 */
CString StringToTchar(const std::string& str);

/*
 *  StringSplitter：将一个字符串根据分隔符切成若干片段
 *  @str：待切分的字符串，输入参数
 *  @splitter：分隔符字符串，输入参数
 *  @return：将str以splitter为分割符进行切分之后的结果
 */
std::vector<std::string> StringSplitter(const std::string& str, const std::string& splitter);

/*
 *  StringToNumber：将一个字符串转换成整数（如果是负数则要求第一个非空白字符为“-”）
 *  @numStr：数字的字符串，输入参数
 *  @remainStr：转换之后剩下的未处理字符串，输出参数
 *  @return：将numStr转换成整数之后的结果，
 *           如果其中有非数字字符（包括小数点，以x举例），
 *           则会输出从高位开始到x之前的转换结果，
 *           如果第一个非空白字符就是非数字字符，则输出0
 */
int32_t StringToInteger(const std::string& numStr, std::string& remainStr = std::string());

/*
 *  StringToDouble：将一个字符串转换成double（如果是负数则要求第一个非空白字符为“-”）
 *  @numStr：数字的字符串，输入参数
 *  @remainStr：转换之后剩下的未处理字符串，输出参数
 *  @return：将numStr转换成double之后的结果，
 *           如果其中有非数字字符（以x举例），
 *           则会输出从高位开始到x之前的转换结果，
 *           如果第一个非空白字符就是非数字字符，则输出0
 *           如果数字字符串中没有小数部分，仍然可以正常解析
 */
double StringToDouble(const std::string& numStr, std::string& remainStr = std::string());

/*
 *  BoolToString：将一个bool值转换为字符串
 *  @boolVal：bool值，输入参数
 *  @return：将boolVal转换成string之后的结果，如果boolVal是true则返回"true"，否则返回"false"
 */
std::string BoolToString(bool boolVal);

/*
 *  StringToBool：将一个字符串转换成bool
 *  @boolStr：bool字符串，输入参数
 *  @return：将boolStr转换成bool之后的结果，如果boolStr是"true"则返回true，否则返回false
 */
bool StringToBool(const std::string& boolStr);

/*
 *  WriteToFile：将一个字符串写到指定文件中，可以指定覆盖或追加
 *  @fileName：要写入文件的文件名，输入参数
 *  @context：待写入的字符串，输入参数
 *  @isAppend：是否以追加的方式写入文件，输入参数，默认为false，即不追加而覆盖
 *  @return：当写入成功时返回一个正数，表示写入的数据大小；当写入失败时返回负数的错误码
 */
int32_t WriteToFile(const std::string& fileName, const std::string& context, bool isAppend = false);

/*
 *  ReadFromFile：从指定文件中读取所有内容到字符串
 *  @fileName：要读取文件的文件名，输入参数
 *  @context：存放文件内容的字符串，输出参数
 *  @return：当读取成功时返回一个正数，表示读取的数据大小；当读取失败时返回负数的错误码
 */
int32_t ReadFromFile(const std::string& fileName, std::string& context);

/*
 *  CalculateSumFromDataVector：计算一个数据向量的和，模版函数
 *  @T1：数据向量中的数据类型的迭代器
 *  @T2：数据向量中待统计数据分量的类型
 *  @beginIter：统计开始位置迭代器
 *  @endIter：统计结束位置迭代器，必须大于等于beginIter
 *  @offset：待统计数据分量在数据类型中的起始位置偏移量
 *  @return：统计区间[beginIter, endIter]对于数据分量T2的求和
 */
template <typename T1, typename T2>
double CalculateSumFromDataVector(const T1& beginIter,
								  const T1& endIter,
								  size_t offset)
{
	double sum = 0.0;
	T1 it(beginIter);
	do
	{
		char * ptr = (char *)&(*it);
		ptr += offset;
		T2 value = *((T2 *)ptr);
		sum += value;
		if (it == endIter)
		{
			break;
		}
		++it;
	}
	while(true);
	return sum;
}

/*
 *  CalculateMeanValueFromDataVector：计算一个数据向量的平均值，模版函数
 *  @T1：数据向量中的数据类型的迭代器
 *  @T2：数据向量中待统计数据分量的类型
 *  @beginIter：统计开始位置迭代器
 *  @endIter：统计结束位置迭代器，必须大于等于beginIter
 *  @offset：待统计数据分量在数据类型中的起始位置偏移量
 *  @return：统计区间[beginIter, endIter]对于数据分量T2的平均值
 */
template <typename T1, typename T2>
double CalculateMeanValueFromDataVector(const T1& beginIter,
										const T1& endIter,
										size_t offset)
{
	double sum = CalculateSumFromDataVector<T1, T2>(beginIter, endIter, offset);
	return sum / (endIter - beginIter + 1);
}

/*
 *  CalculateStandardDeviationFromDataVector：计算一个数据向量的标准差，模版函数
 *  @T1：数据向量中的数据类型的迭代器
 *  @T2：数据向量中待统计数据分量的类型
 *  @beginIter：统计开始位置迭代器
 *  @endIter：统计结束位置迭代器，必须大于等于beginIter
 *  @meanValue：该统计区间数据的均值
 *  @offset：待统计数据分量在数据类型中的起始位置偏移量
 *  @return：统计区间[beginIter, endIter]对于数据分量T2的标准差
 */
template <typename T1, typename T2>
double CalculateStandardDeviationFromDataVector(const T1& beginIter,
												const T1& endIter,
												double meanValue,
												size_t offset)
{
	double sum = 0.0;
	size_t count(0);
	T1 it(beginIter);
	do
	{
		char * ptr = (char *)&(*it);
		ptr += offset;
		T2 value = *((T2 *)ptr);
		double delta = value - meanValue;
		sum += (delta * delta);
		++count;
		if (it == endIter)
		{
			break;
		}
		++it;
	}
	while(true);
	return sqrt(sum / count);
}

#endif //UTILITY_H