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
#include <algorithm>
#include <boost/algorithm/string.hpp>

const std::string operators[] = {
"+=",
"-=",
"*=",
"/=",
":="
};
bool isValidOperator(std::string type);
short int getOperatorIndex(std::string type);

const std::string operators2[] = {
//logic
"||",
"&&",
"<<",
">>",
">=",
"<=",
"!=",
"=="
};
bool isValidOperator2(std::string type);
short int getOperator2Index(std::string type);

const std::string variable_type[] = {
"invalid",
"null",
"int",
"float",
"bool",
"instance",
"object",
"sprite",
"texture",
"sound",
"music",
"font",
"point",
"rectangle",
"color",
"string",
"enum"
};
bool isValidVariable(std::string type);
short int getVariableIndex(std::string type);

const std::string keywords[] = {
"set",
"if",
"end",
"else",
};
bool isValidKeyword(std::string type);
short int getKeywordIndex(std::string type);

