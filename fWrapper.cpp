#include "fWrapper.h"

#include <fstream>
#include <iostream>

#include "Func.h"
#include "Members.h"

fWrapper* fWrapper::_instance = nullptr;

fWrapper::fWrapper()
{
	this->_functions = std::vector<function>();
}

void fWrapper::Init()
{
	_instance = new fWrapper();
};

bool fWrapper::AddLib(const std::string& lib_name)
{
	if (_instance == nullptr) return false;
	std::ifstream t(lib_name);
	if (!t.good())
	{
		std::cout << "Error: file '" << lib_name << "' not found" << std::endl;
		return false;
	}
	std::string tmp((std::istreambuf_iterator<char>(t)), std::istreambuf_iterator<char>());
	std::vector<std::string> Alib = Explode(tmp, '\n');
	t.close();
	int i = 0;
	for (std::string line : Alib)
	{
		if (IsComment(line)) continue;
		std::vector<std::string> tokens = MakeTokens(line, true);
		if (tokens.size() >= 4)
		{
			//point new_point(float x,float y)
			std::string f_name = tokens[1];
			variable f_return;
			auto f_arguments = std::vector<variable>();

			if (isValidVariable(tokens[0]))
			{
				f_return.Name = "return";
				f_return.Type = tokens[0];
			}

			if (tokens.size() > 4)
			{
				for (int i = 3; i < tokens.size() - 1; i += 3)
				{
					if (i + 1 < static_cast<int>(tokens.size()))
					{
						f_arguments.emplace_back(tokens[i + 1], tokens[i]);
					}
				}
			}
			_instance->_functions.emplace_back(f_name, f_return, f_arguments, i++);
		}
	}
	return true;
}

function* fWrapper::GetFunction(const std::string& name)
{
	for (auto& function : _instance->_functions)
	{
		if (function.f_name == name) return &function;
	}
	return nullptr;
}

int fWrapper::GetFunction_id(const std::string& name)
{
	for (int i = 0; i < _instance->_functions.size(); i++)
	{
		if (_instance->_functions[i].f_name == name) return i;
	}
	return -1;
}

void fWrapper::PrintAllFunctions()
{
	for (function& f : _instance->_functions)
	{
		std::cout << f.f_name << " return: " << f.f_return.Type << " takes: ";
		if (!f.f_arguments.empty())
		{
			for (auto& f_argument : f.f_arguments)
			{
				std::cout << f_argument.Name << "[" << f_argument.Type << "] ";
			}
		}
		else
		{
			std::cout << "nothing";
		}
		std::cout << std::endl;
	}
}
