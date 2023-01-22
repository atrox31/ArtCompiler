#pragma once
#include <string>
#include <vector>
#include <codecvt>
#include <vector>

#include "function.h"

class fWrapper
{
	std::vector<function> _functions;
	static fWrapper* _instance;
	fWrapper();

public:
	static void Init();
	static bool AddLib(const std::string& lib_name);
	static function* GetFunction(const std::string& name);
	static int GetFunction_id(const std::string& name);
	static void PrintAllFunctions();
};
