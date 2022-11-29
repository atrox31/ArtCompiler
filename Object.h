#pragma once
#include <vector>
#include "varible.h"
#include <string>
#include <map>

class Object {
	std::vector<varible> _locals;
public:
	int CodeId = -1;
	std::map<std::string, std::string> Functions;
	std::string Name = "undefined";
	Object();
	void SetLocal(std::string name, std::string type, bool readonly = false);
	varible* FindLocal(std::string name);
	std::vector<varible> GetLocals();
};

