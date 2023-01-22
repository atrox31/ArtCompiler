#include "oWrapper.h"

#include <fstream>
#include <iostream>

#include "Func.h"

extern int c_line;
extern std::string c_func;
extern std::string c_object;

oWrapper* oWrapper::_instance;
std::vector<Object*> oWrapper::_obj;
int oWrapper::c_CodeId;

void oWrapper::Init()
{
	_instance = new oWrapper();
	_obj = std::vector<Object*>();
	c_CodeId = 0;
}

oWrapper::oWrapper()
{
	_instance = this;
}

bool oWrapper::CreateObject(const std::string& file)
{
	c_line = -1;
	c_func = "init";
	c_object = "undefined";

	if (_instance == nullptr) return false;
	std::ifstream t(file);
	if (!t.good())
	{
		std::cout << "Error: file '" << file << "' not found" << std::endl;
		return false;
	}
	std::string tmp((std::istreambuf_iterator<char>(t)), std::istreambuf_iterator<char>());
	std::vector<std::string> objc = Explode(tmp, '\n');
	t.close();

	bool object_definition = false;
	bool function_definition = false;
	std::string currentFunction;

	auto _new_object = new Object();
	for (std::string line : objc)
	{
		c_line++;

		std::vector<std::string> tokens = MakeTokens(line);

		if (tokens[0] == "object")
		{
			_new_object->Name = tokens[1];
			c_object = tokens[1];
			object_definition = true;
			continue;
		}

		if (object_definition)
		{
			if (tokens[0] == "local")
			{
				std::string name, type;
				name = tokens[2];
				type = tokens[1];
				_new_object->SetLocal(name, type);
				continue;
			}

			if (tokens[0] == "define")
			{
				_new_object->Functions.insert(std::make_pair(tokens[1], ""));
				continue;
			}

			if (tokens[0] == "@end")
			{
				object_definition = false;
				continue;
			}
		}

		if (tokens[0] == "function")
		{
			function_definition = true;
			currentFunction = Explode(tokens[1], ':').back();
			continue;
		}

		if (function_definition)
		{
			if (tokens[0] == "@end")
			{
				function_definition = false;
				currentFunction = "";
				continue;
			}
			if (IsComment(line))
			{
				// add empty line to get correct line number
				_new_object->Functions[currentFunction] += '\n';
			}
			else
			{
				_new_object->Functions[currentFunction] += line + '\n';
			}
		}
	}
	_new_object->CodeId = c_CodeId++;
	_obj.push_back(_new_object);
	return true;
}

std::vector<Object*> oWrapper::GetObjects()
{
	return _obj;
}

Object* oWrapper::GetObjectByName(const std::string& name)
{
	for (const auto& obj : _obj)
	{
		if (obj->Name == name)
		{
			return obj;
		}
	}
	return nullptr;
}
