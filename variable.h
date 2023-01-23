#pragma once
#include <string>

class variable
{
public:
	std::string Name;
	std::string Type;
	int Index;
	variable(std::string name, std::string type);
	variable(variable& var, int index);
	variable();
};
