#pragma once
#include <string>

class variable {
public:
	std::string Name;
	std::string Type;
	int index;
	bool ReadOnly;
	variable(std::string name, std::string type, bool readonly = false);
	variable(variable& var, int index);
	variable();
};



