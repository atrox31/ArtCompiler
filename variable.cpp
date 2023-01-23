#include "variable.h"

variable::variable(std::string name, std::string type)
{
	this->Name = std::move(name);
	this->Type = std::move(type);
	this->Index = -1;
}

variable::variable(variable& var, const int index)
{
	this->Name = var.Name;
	this->Type = var.Type;
	this->Index = index;
}

variable::variable()
{
	this->Name = "undefined";
	this->Type = "undefined";
	this->Index = -1;
}
