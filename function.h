#pragma once
#include <unordered_map>
#include <fstream>
#include <math.h>
#include <sstream>
#include <iomanip>
#include <codecvt>
#include <iostream>
#include <vector>
#include <fstream>
#include <streambuf>
#include<algorithm>

#include "varible.h"

class function {
public:
	std::string f_name;
	varible f_return;
	std::vector<varible> f_arguments;
	int index;
	function(std::string f_name, varible f_return, std::vector<varible> f_arguments, int index) {
		this->f_name = f_name;
		this->f_return = f_return;
		this->f_arguments = f_arguments;
		this->index = index;
	}
};
