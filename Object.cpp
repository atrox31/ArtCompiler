#include "Object.h"
#include "Func.h"

Object::Object() {
	_locals = std::vector<variable>();
	 Functions = std::map<std::string, std::string>();
}
void Object::SetLocal(const std::string& name, const std::string& type) {
	if (FindLocal(name) != nullptr) {
		Error("variable '" + name + "' exists in this object", 1);
		return;
	}
	_locals.emplace_back(name, type);
	int c = -1;
	for (auto& local : _locals)
	{
		if (local.Type == type) c++;
	}
	_locals.back().index = c;
}

variable* Object::FindLocal(const std::string& name) {
	for (auto& local : _locals)
	{
		if (local.Name == name) return &local;
	}
	return nullptr;
}

std::vector<variable> Object::GetLocals()
{
	return _locals;
}
