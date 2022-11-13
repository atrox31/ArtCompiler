#pragma once
#include <string>

class varible {
public:
	std::string Name;
	std::string Type;
	int index;
	bool ReadOnly;
	varible(std::string name, std::string type, bool readonly = false);
	varible(varible& var, int index);
	varible();
};



