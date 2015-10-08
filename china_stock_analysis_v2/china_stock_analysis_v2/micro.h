#ifndef __MICRO_DEFINE__
#define __MICRO_DEFINE__
//number micro
#define TEMP_BUFFER_SIZE 1024
//function micro
#define DOUBLE_IS_ZERO(x) ((x) < 0.000001 && (x) > -0.000001)
#define DEFINE_TEMP_BUFFER(buf) char buf[TEMP_BUFFER_SIZE];

#endif //__MICRO_DEFINE__

#ifndef __CONST_DEFINE__
#define __CONST_DEFINE__
#include <string>
#include <stdint.h>

const std::string StableDataDir = "stable_data\\";
const std::string CacheDataDir = "cache_data\\";
const std::string ResultDir = "result\\";
const std::string BaslUrlOfSinaSource = "http://biz.finance.sina.com.cn/stock/flash_hq/kline_data.php?&rand=random(10000)&";
const std::string BatchAnalysisFile = StableDataDir + "��������.txt";
const std::string BatchPredictFile = StableDataDir + "����Ԥ��.txt";
const std::string BatchAnalysisResultFile = ResultDir + "�����������.txt";
const std::string BatchPredictsResultFile = ResultDir + "����Ԥ����.txt";
const std::string ShenZhenStockMarketID = "sz399001"; //��ָ֤���Ĺ�ƱID
const std::string ShangHaiStockMarketID = "sh000001"; //��ָ֤���Ĺ�ƱID

#endif //__CONST_DEFINE__