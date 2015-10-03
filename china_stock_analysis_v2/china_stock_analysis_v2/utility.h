#ifndef UTILITY_H
#define UTILITY_H
#include <string>
#include <vector>
#include <stdint.h>

/*
 *  TcharToString����һ��TCHAR�ַ���ת��Ϊstd::string�ַ���
 *  @tcharString��һ��TCHAR�ַ������������
 *  @return���������ַ������Ӧ��std::string�ַ���
 */
std::string TcharToString(const TCHAR * tcharString);

/*
 *  StringToTchar����һ��char*�ַ���ת��ΪCString�ַ���
 *  @str��һ��char*�ַ������������
 *  @return���������ַ������Ӧ��CString�ַ���
 */
CString StringToTchar(const char *str);
/*
 *  StringToTchar��std::string�����
 *  @str��һ��std::string�ַ������������
 *  @return���������ַ������Ӧ��CString�ַ���
 */
CString StringToTchar(const std::string& str);

/*
 *  StringSplitter����һ���ַ������ݷָ����г�����Ƭ��
 *  @str�����зֵ��ַ������������
 *  @splitter���ָ����ַ������������
 *  @return����str��splitterΪ�ָ�������з�֮��Ľ��
 */
std::vector<std::string> StringSplitter(const std::string& str, const std::string& splitter);

/*
 *  StringToNumber����һ���ַ���ת��������������Ǹ�����Ҫ���һ���ǿհ��ַ�Ϊ��-����
 *  @numStr�����ֵ��ַ������������
 *  @remainStr��ת��֮��ʣ�µ�δ�����ַ������������
 *  @return����numStrת��������֮��Ľ����
 *           ��������з������ַ�������С���㣬��x��������
 *           �������Ӹ�λ��ʼ��x֮ǰ��ת�������
 *           �����һ���ǿհ��ַ����Ƿ������ַ��������0
 */
int32_t StringToInteger(const std::string& numStr, std::string& remainStr = std::string());

/*
 *  StringToDouble����һ���ַ���ת����double������Ǹ�����Ҫ���һ���ǿհ��ַ�Ϊ��-����
 *  @numStr�����ֵ��ַ������������
 *  @remainStr��ת��֮��ʣ�µ�δ�����ַ������������
 *  @return����numStrת����double֮��Ľ����
 *           ��������з������ַ�����x��������
 *           �������Ӹ�λ��ʼ��x֮ǰ��ת�������
 *           �����һ���ǿհ��ַ����Ƿ������ַ��������0
 *           ��������ַ�����û��С�����֣���Ȼ������������
 */
double StringToDouble(const std::string& numStr, std::string& remainStr = std::string());

/*
 *  BoolToString����һ��boolֵת��Ϊ�ַ���
 *  @boolVal��boolֵ���������
 *  @return����boolValת����string֮��Ľ�������boolVal��true�򷵻�"true"�����򷵻�"false"
 */
std::string BoolToString(bool boolVal);

/*
 *  StringToBool����һ���ַ���ת����bool
 *  @boolStr��bool�ַ������������
 *  @return����boolStrת����bool֮��Ľ�������boolStr��"true"�򷵻�true�����򷵻�false
 */
bool StringToBool(const std::string& boolStr);

/*
 *  WriteToFile����һ���ַ���д��ָ���ļ��У�����ָ�����ǻ�׷��
 *  @fileName��Ҫд���ļ����ļ������������
 *  @context����д����ַ������������
 *  @isAppend���Ƿ���׷�ӵķ�ʽд���ļ������������Ĭ��Ϊfalse������׷�Ӷ�����
 *  @return����д��ɹ�ʱ����һ����������ʾд������ݴ�С����д��ʧ��ʱ���ظ����Ĵ�����
 */
int32_t WriteToFile(const std::string& fileName, const std::string& context, bool isAppend = false);

/*
 *  ReadFromFile����ָ���ļ��ж�ȡ�������ݵ��ַ���
 *  @fileName��Ҫ��ȡ�ļ����ļ������������
 *  @context������ļ����ݵ��ַ������������
 *  @return������ȡ�ɹ�ʱ����һ����������ʾ��ȡ�����ݴ�С������ȡʧ��ʱ���ظ����Ĵ�����
 */
int32_t ReadFromFile(const std::string& fileName, std::string& context);

/*
 *  CalculateSumFromDataVector������һ�����������ĺͣ�ģ�溯��
 *  @T1�����������е��������͵ĵ�����
 *  @T2�����������д�ͳ�����ݷ���������
 *  @beginIter��ͳ�ƿ�ʼλ�õ�����
 *  @endIter��ͳ�ƽ���λ�õ�������������ڵ���beginIter
 *  @offset����ͳ�����ݷ��������������е���ʼλ��ƫ����
 *  @return��ͳ������[beginIter, endIter]�������ݷ���T2�����
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
 *  CalculateMeanValueFromDataVector������һ������������ƽ��ֵ��ģ�溯��
 *  @T1�����������е��������͵ĵ�����
 *  @T2�����������д�ͳ�����ݷ���������
 *  @beginIter��ͳ�ƿ�ʼλ�õ�����
 *  @endIter��ͳ�ƽ���λ�õ�������������ڵ���beginIter
 *  @offset����ͳ�����ݷ��������������е���ʼλ��ƫ����
 *  @return��ͳ������[beginIter, endIter]�������ݷ���T2��ƽ��ֵ
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
 *  CalculateStandardDeviationFromDataVector������һ�����������ı�׼�ģ�溯��
 *  @T1�����������е��������͵ĵ�����
 *  @T2�����������д�ͳ�����ݷ���������
 *  @beginIter��ͳ�ƿ�ʼλ�õ�����
 *  @endIter��ͳ�ƽ���λ�õ�������������ڵ���beginIter
 *  @meanValue����ͳ���������ݵľ�ֵ
 *  @offset����ͳ�����ݷ��������������е���ʼλ��ƫ����
 *  @return��ͳ������[beginIter, endIter]�������ݷ���T2�ı�׼��
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