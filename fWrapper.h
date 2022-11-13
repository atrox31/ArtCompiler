#pragma once
#include <string>
#include <vector>
#include <map>
#include <unordered_map>
#include <fstream>
#include <math.h>
#include <sstream>
#include <iomanip>
#include <codecvt>
#include <iostream>
#include <vector>
#include <fstream>
#include <streambuf>
#include<algorithm>

#include "function.h"

class fWrapper {
	std::vector<function> _functions;
	static fWrapper* _instance;
	fWrapper();
public:
	static void Init();
	static bool AddLib(std::string LibName);
	static function* GetFunction(std::string name);
	static int GetFunction_id(std::string name);
	static void PrintAllFunctions();
};