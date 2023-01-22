#pragma once
#include <vector>
#include "variable.h"
#include <string>
#include <map>

class Object
{
	std::vector<variable> _locals;

public:
	int CodeId = -1;
	std::map<std::string, std::string> Functions;
	std::string Name = "undefined";
	Object();
	void SetLocal(const std::string& name, const std::string& type);
	variable* FindLocal(const std::string& name);
	std::vector<variable> GetLocals();
};
