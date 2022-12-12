#include "Members.h"

#define ARRAY_SIZE(a) (sizeof(a)/sizeof(a[0]))

bool isValidOperator(std::string type) {
	for (int i = 0; i < ARRAY_SIZE(operators); i++) if (boost::algorithm::to_lower_copy(type) == operators[i]) return true;
	return false;
}
short int getOperatorIndex(std::string type)
{
	for (short int i = 0; i < ARRAY_SIZE(operators); i++) if (type == operators[i]) return i;
	return -1;
}
bool isValidOperator2(std::string type) {
	for (int i = 0; i < ARRAY_SIZE(operators2); i++) if (type == operators2[i]) return true;
	return false;
}
short int getOperator2Index(std::string type)
{
	for (short int i = 0; i < ARRAY_SIZE(operators2); i++) if (type == operators2[i]) return i;
	return -1;
}
bool isValidVariable(std::string type) {
	for (int i = 0; i < ARRAY_SIZE(variable_type); i++) if (boost::algorithm::to_lower_copy(type) == variable_type[i]) return true;
	return false;
}
short int getVariableIndex(std::string type)
{
	for (short int i = 0; i < ARRAY_SIZE(variable_type); i++) if (boost::algorithm::to_lower_copy(type) == variable_type[i]) return i;
	return -1;
}
bool isValidKeyword(std::string type) {
	for (int i = 0; i < ARRAY_SIZE(keywords); i++) if (boost::algorithm::to_lower_copy(type) == keywords[i]) return true;
	return false;
}

short int getKeywordIndex(std::string type)
{
	for (short int i = 0; i < ARRAY_SIZE(keywords); i++) if (boost::algorithm::to_lower_copy(type) == keywords[i]) return i;
	return -1;
}
