#pragma once
#include <string>

const std::string operators[] = {
	"+=",
	"-=",
	"*=",
	"/=",
	":="
};
bool isValidOperator(const std::string& type);
short int getOperatorIndex(const std::string& type);

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
bool isValidOperator2(const std::string& type);
short int getOperator2Index(const std::string& type);

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
bool isValidVariable(const std::string& type);
short int getVariableIndex(const std::string& type);

const std::string keywords[] = {
	"set",
	"if",
	"end",
	"else",
};
bool isValidKeyword(const std::string& type);
short int getKeywordIndex(const std::string& type);
