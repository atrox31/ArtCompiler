#pragma once
#include <iomanip>
#include <codecvt>
#include <utility>
#include <vector>
#include<algorithm>

#include "variable.h"

class function {
public:
	std::string f_name;
	variable f_return;
	std::vector<variable> f_arguments;
	int index;
	function(std::string f_name, variable f_return, std::vector<variable> f_arguments, int index) {
		this->f_name = std::move(f_name);
		this->f_return = std::move(f_return);
		this->f_arguments = std::move(f_arguments);
		this->index = index;
	}
};
