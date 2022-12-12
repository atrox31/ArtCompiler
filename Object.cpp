#include "Object.h"
#include "Func.h"

Object::Object() {
	_locals = std::vector<variable>();
	 Functions = std::map<std::string, std::string>();
}
void Object::SetLocal(std::string name, std::string type, bool readonly) {
	if (FindLocal(name) != nullptr) {
		Error("variable '" + name + "' exists in this object", 1);
		return;
	}
	_locals.push_back(variable(name, type, readonly));
	int c = -1;
	for (int i = 0; i < _locals.size(); i++) {
		if (_locals[i].Type == type) c++;
	}
	_locals.back().index = c;
}

variable* Object::FindLocal(std::string name) {
	for (int i = 0; i < _locals.size(); i++) {
		if (_locals[i].Name == name) return &_locals[i];
	}
	return nullptr;
}

std::vector<variable> Object::GetLocals()
{
	return _locals;
}
