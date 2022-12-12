#include "Members.h"

bool isValidOperator(const std::string& type) {
	for (const auto& i : operators) if (boost::algorithm::to_lower_copy(type) == i) return true;
	return false;
}

short int getOperatorIndex(const std::string& type) {
	for (short int i = 0; i < std::size(operators); i++) if (type == operators[i]) return i;
	return -1;
}

bool isValidOperator2(const std::string& type) {
	for (const auto& i : operators2) if (type == i) return true;
	return false;
}

short int getOperator2Index(const std::string& type)
{
	for (short int i = 0; i < std::size(operators2); i++) if (type == operators2[i]) return i;
	return -1;
}

bool isValidVariable(const std::string& type) {
	for (const auto& i : variable_type) if (boost::algorithm::to_lower_copy(type) == i) return true;
	return false;
}

short int getVariableIndex(const std::string& type)
{
	for (short int i = 0; i < std::size(variable_type); i++) if (boost::algorithm::to_lower_copy(type) == variable_type[i]) return i;
	return -1;
}

bool isValidKeyword(const std::string& type) {
	for (const auto& keyword : keywords) if (boost::algorithm::to_lower_copy(type) == keyword) return true;
	return false;
}

short int getKeywordIndex(const std::string& type)
{
	for (short int i = 0; i < std::size(keywords); i++) if (boost::algorithm::to_lower_copy(type) == keywords[i]) return i;
	return -1;
}
