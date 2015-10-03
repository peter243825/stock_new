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
const std::string ResultFile = ResultDir + "result.txt";
const std::string ShenZhenStockMarketID = "399001.SZ"; //深证指数的股票ID
const std::string ShangHaiStockMarketID = "000001.SS"; //上证指数的股票ID

#endif //__CONST_DEFINE__