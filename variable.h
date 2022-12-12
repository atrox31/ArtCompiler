#pragma once
#include <string>

class variable {
public:
	std::string Name;
	std::string Type;
	int index;
	variable(std::string name, std::string type);
	variable(variable& var, int index);
	variable();
};



