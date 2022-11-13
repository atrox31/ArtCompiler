#include "Object.h"
#include "Func.h"

Object::Object() {
	_locals = std::vector<varible>();
	 Functions = std::map<std::string, std::string>();
}
void Object::SetLocal(std::string name, std::string type, bool readonly) {
	if (FindLocal(name) != nullptr) {
		Error("Varible '" + name + "' exists in this object");
		return;
	}
	_locals.push_back(varible(name, type, readonly));
	int c = -1;
	for (int i = 0; i < _locals.size(); i++) {
		if (_locals[i].Type == type) c++;
	}
	_locals.back().index = c;
}

varible* Object::FindLocal(std::string name) {
	for (int i = 0; i < _locals.size(); i++) {
		if (_locals[i].Name == name) return &_locals[i];
	}
	return nullptr;
}

std::vector<varible> Object::GetLocals()
{
	return _locals;
}
