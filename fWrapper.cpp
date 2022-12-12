#include "fWrapper.h"
#include "Func.h"
#include "Members.h"

fWrapper* fWrapper::_instance = nullptr;
fWrapper::fWrapper() {
	this->_functions = std::vector<function>();
}

void fWrapper::Init() {
	fWrapper::_instance = new fWrapper();
};

bool fWrapper::AddLib(std::string LibName) {
	if (_instance == nullptr) return false;
	std::ifstream t(LibName);
	if (!t.good()) {
		std::cout << "Error: file '" << LibName << "' not found" << std::endl;
		return false;
	}
	std::string tmp((std::istreambuf_iterator<char>(t)), std::istreambuf_iterator<char>());
	std::vector<std::string> Alib = Explode(tmp, '\n');
	t.close();
	int i = 0;
	for (std::string line : Alib) {
		if (IsComment(line)) continue;
		std::vector<std::string> tokens = MakeTokens(line, true);
		if (tokens.size() >= 4) {
			//point new_point(float x,float y)
			std::string f_name = tokens[1];
			variable f_return;
			std::vector<variable> f_argumens = std::vector<variable>();

			if (isValidVariable(tokens[0])) {
				f_return.Name = "return";
				f_return.Type = tokens[0];
				f_return.ReadOnly = false;
			}

			if (tokens.size() > 4) {
				for (int i = 3; i < tokens.size() - 1; i += 3) {
					if (i + 1 < (int)tokens.size()) {
						f_argumens.push_back(variable(tokens[i + 1], tokens[i], true));
					}
				}
			}
			_instance->_functions.push_back(function(f_name, f_return, f_argumens, i++));
		}
	}
	return true;
}

function* fWrapper::GetFunction(std::string name) {
	for (int i = 0; i < _instance->_functions.size(); i++) {
		if (_instance->_functions[i].f_name == name) return &_instance->_functions[i];
	}
	return nullptr;
}

int fWrapper::GetFunction_id(std::string name)
{
	for (int i = 0; i < _instance->_functions.size(); i++) {
		if (_instance->_functions[i].f_name == name) return i;
	}
	return -1;
}

void fWrapper::PrintAllFunctions()
{
	for (function& f : _instance->_functions) {
		std::cout << f.f_name << " return: " << f.f_return.Type << " takes: ";
		if (f.f_arguments.size() > 0) {
			for (int i = 0; i < f.f_arguments.size(); i++) {
				std::cout << f.f_arguments[i].Name << "[" << f.f_arguments[i].Type << "] ";
			}
		}
		else {
			std::cout << "nothing";
		}
		std::cout << std::endl;
	}
}
