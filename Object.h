#pragma once
#include <vector>
#include "variable.h"
#include <string>
#include <map>

class Object {
	std::vector<variable> _locals;
public:
	int CodeId = -1;
	std::map<std::string, std::string> Functions;
	std::string Name = "undefined";
	Object();
	void SetLocal(std::string name, std::string type, bool readonly = false);
	variable* FindLocal(std::string name);
	std::vector<variable> GetLocals();
};

