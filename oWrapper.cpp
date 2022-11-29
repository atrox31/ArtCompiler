#include "oWrapper.h"

extern int cLine;
extern std::string cFunc;
extern std::string cObject;

oWrapper* oWrapper::_instance;
std::vector<Object*> oWrapper::_obj;
int oWrapper::c_CodeId;

void oWrapper::Init() {
	oWrapper::_instance = new oWrapper();
	oWrapper::_obj = std::vector<Object*>();
	oWrapper::c_CodeId = 0;
}

oWrapper::oWrapper() {
	oWrapper::_instance = this;
}

bool oWrapper::CreateObject(std::string file) {
	cLine = -1;
	cFunc = "init";
	cObject = "undefined";

	if (_instance == nullptr) return false;
	std::ifstream t(file);
	if (!t.good()) {
		std::cout << "Error: file '" << file << "' not found" << std::endl;
		return false;
	}
	std::string tmp((std::istreambuf_iterator<char>(t)), std::istreambuf_iterator<char>());
	std::vector<std::string> objc = Explode(tmp, '\n');
	t.close();

	bool object_definition = false;
	bool function_definition = false;
	std::string currentFunction = "";

	Object* _new_object = new Object();
	for (std::string line : objc) {
		cLine++;
		if (IsComment(line)) continue;
		std::vector<std::string> tokens = MakeTokens(line);

		if (tokens[0] == "object") {
			_new_object->Name = tokens[1];
			cObject = tokens[1];
			object_definition = true;
			continue;
		}

		if (object_definition) {

			if (tokens[0] == "local") {
				bool ro = (tokens[1] == "READ_ONLY");
				std::string name, type;
				if (ro) {
					name = tokens[3];
					type = tokens[2];
				}
				else {
					name = tokens[2];
					type = tokens[1];
				}
				_new_object->SetLocal(name, type, ro);
				continue;
			}

			if (tokens[0] == "function") {
				_new_object->Functions.insert(std::make_pair(tokens[1], ""));
				continue;
			}

			if (tokens[0] == "@end") {
				object_definition = false;
			}
		}

		if (tokens[0] == "function") {
			function_definition = true;
			currentFunction = Explode(tokens[1], ':').back();
			continue;
		}

		if (function_definition) {
			if (tokens[0] == "@end") {
				function_definition = false;
				currentFunction = "";
				continue;
			}
			_new_object->Functions[currentFunction] += line + '\n';
		}

	}
	_new_object->CodeId = c_CodeId++;
	_obj.push_back(_new_object);
	return true;
}

std::vector<Object*> oWrapper::GetObjects()
{
	return _instance->_obj;
}

Object* oWrapper::GetObjectByName(std::string name)
{
	for (auto& obj : _instance->_obj) {
		if (obj->Name == name) {
			return obj;
		}
	}
	return nullptr;
}

