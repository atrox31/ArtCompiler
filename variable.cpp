#include "variable.h"

#include <utility>

variable::variable(std::string name, std::string type) {
	this->Name = std::move(name);
	this->Type = std::move(type);
	this->index = -1;
}

variable::variable(variable& var, int index)
{
	this->Name = var.Name;
	this->Type = var.Type;
	this->index = index;
}

variable::variable()
{
	this->Name = "undefined";
	this->Type = "undefined";
	this->index = -1;
}
