#pragma once
#include <string>
#include <vector>
#include <map>
#include <unordered_map>
#include <fstream>
#include <math.h>
#include <sstream>
#include <iomanip>
#include <codecvt>
#include <iostream>
#include <vector>
#include <fstream>
#include <streambuf>
#include<algorithm>

#include "Func.h"
#include "Object.h"

class oWrapper {
	static std::vector<Object*> _obj;
	static oWrapper* _instance;
public:
	static int c_CodeId;
	static void Init();
	oWrapper();
	static bool CreateObject(std::string file);
	static std::vector<Object*> GetObjects();
	static Object* GetObjectByName(std::string name);
};
