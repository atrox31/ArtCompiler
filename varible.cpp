#include "varible.h"

varible::varible(std::string name, std::string type, bool readonly) {
	this->Name = name;
	this->Type = type;
	this->ReadOnly = readonly;
	this->index = -1;
}

varible::varible(varible& var, int index)
{
	this->Name = var.Name;
	this->Type = var.Type;
	this->ReadOnly = var.ReadOnly;
	this->index = index;
}

varible::varible()
{
	this->Name = "undefined";
	this->Type = "undefined";
	this->ReadOnly = false;
	this->index = -1;
}
