#pragma once
#include <string>
class CAnalysisEngine
{
public:
	CAnalysisEngine(void);
	~CAnalysisEngine(void);
	static std::string SingleStockAnalysis(const std::string& stockID, const std::string& selectedMonth);
};

