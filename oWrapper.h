#pragma once
#include <string>
#include <vector>
#include <codecvt>
#include <vector>
#include "Object.h"

class oWrapper {
	static std::vector<Object*> _obj;
	static oWrapper* _instance;
public:
	static int c_CodeId;
	static void Init();
	oWrapper();
	static bool CreateObject(const std::string& file);
	static std::vector<Object*> GetObjects();
	static Object* GetObjectByName(const std::string& name);
};
