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
#include <boost/algorithm/string.hpp>
#include <boost/lexical_cast.hpp>

#include "Members.h"
#include "Func.h"
#include "fWrapper.h"
#include "Object.h"
#include "oWrapper.h"

#define ARRAY_SIZE(a) (sizeof(a)/sizeof(a[0]))

int cLine = 0;
std::string cFunc = "none";
std::string cObject = "none";
std::string cLineCode = "none";
std::vector<unsigned char> OutputCode;

#define ALLOW_0_LENGTH_STRING true


template <typename E>
constexpr typename std::underlying_type<E>::type to_underlying(E e) noexcept {
	return static_cast<typename std::underlying_type<E>::type>(e);
}

enum class Command {
	// definicje
	OBJECT_DEFINITION,
	FUNCTION_DEFINITION,
	LOCAL_VARIBLE_DEFINITION,
	TYPEDEF,
	// odwo³ania
	OBJECT,
	OBJECT_VARIBLE,
	LOCAL_VARIBLE,
	VALUE,NULL_VALUE,
	FUNCTION,OTHER,
	// operatory
	SET,
	OPERATOR,
	// polecenia
	TYPE, IF_BODY, IF_TEST, ELSE,
	END,
};

bool TryToParse(std::string value, std::string type) {
	if (type == "int") {
		try {
			std::ignore = std::stof(value);
			return true;
		}
		catch (...) {
			Error("try to parse '" + value + "' to '" + type + "'",2);
			return false;
		}
	}
	else if (type == "float") {
		try{
			std::ignore = std::stof(value);
		return true;
		}
			catch (...) {
			Error("try to parse '" + value + "' to '" + type + "'",3);
			return false;
		}
	}
	else if (type == "bool") {
		boost::algorithm::to_lower(value);
		if (value == "true" || value == "false") return true;
		Error("try to parse '" + value + "' to '" + type + "'",4);
		return false;
	}
	else if (type == "string") {
#if !ALLOW_0_LENGTH_STRING
		if (value.length() == 0) {
			Error("try to parse '" + value + "' to '" + type + "'",5);
			return false;
		}
#endif
		return true;
	}
	else if (type == "color") {
		const int len = (int)value.length();
		// rgb or rgba
		if ( !(len == 7 || len == 9)) return false;
		if (value[0] != '#') return false;
		for (int i = 1; i < len; i++) {
			if ((
				(value[i] >= '0' && value[i] <= '9') ||
				(value[i] >= 'a' && value[i] <= 'f') ||
				(value[i] >= 'A' && value[i] <= 'F')
				) == false) return false;
		}
		return true;
	}
	else if (type == "point") {
		
		auto point = Split(value, ':');
		if (point.size() != 2) { Error("try to parse " + value + " to " + type + " - error",6); return false; }
		if (!TryToParse(point[0], "float")){ Error("point[0] of '" + value + "' value='"+ point[0] + "' - error",7); return false; }
		if (!TryToParse(point[1], "float")){ Error("point[1] of '" + value + "' value='"+ point[1] + "' - error",8); return false; }
		return true;
	}
	Error("type '"+ type+"' is not supperted yet.",15);
	return false;
}

void WriteString(std::string text) {
	if (text.length() == 0) return;
	for (int i = 0; i < text.length(); i++) {
		OutputCode.push_back((unsigned char)text[i]);
	}
	OutputCode.push_back('\1');
}

void WriteBit(short int value) {
	OutputCode.push_back((unsigned char)value);
}

void WriteCommand(Command value) {
	OutputCode.push_back((unsigned char)to_underlying(value));
}

void WriteValue(std::string type, short int value) {
	if (type.length() == 0) return;
	short int _type = getVaribleIndex(type);
	OutputCode.push_back((unsigned char)_type);
	OutputCode.push_back((unsigned char)value);
}


class TokenCompiller {

	std::vector<std::string> _tokens;
	std::vector<std::string> _code;
	int postion;
	int postion_max;
	std::vector<int> line;
	std::vector<int> skip_ptr;
	std::string current;

public:
	TokenCompiller(std::string code) {
		line = std::vector<int>();
		skip_ptr = std::vector<int>();
		_tokens = std::vector<std::string>();
		_code = std::vector<std::string>();
		for (std::string code : Explode(code, '\n')) {
			std::vector<std::string> tokens = MakeTokens(code);
			_tokens.insert(_tokens.end(), tokens.begin(), tokens.end());
			line.push_back((int)tokens.size() - 1);
			_code.push_back(code);
		}
		postion = -1;
		postion_max = (int)_tokens.size() - 1;
	}

	std::string GetCode(int i) {
		return _code[i];
	}

	int GetLine() {
		int cline = 0;
		int cpos = 0;
		for (int i = 0; i < line.size() - 1; i++) {
			cpos += line[i];
			cline++;
			if (cpos >= postion) {
				return cline;
			}
		}
		return 0;
	}

	void EnterNextScope() {
		WriteBit(0);
		skip_ptr.push_back((int)OutputCode.size() - 1);
	}
	bool ExitScope() {
		int skip = ((int)OutputCode.size() - 1) - skip_ptr.back();
		if (skip > 254) {
			Error("scope is too long! ("+std::to_string(skip)+") max 254",9);
			return false;
		}
		OutputCode[skip_ptr.back()] = (unsigned char)skip;
		skip_ptr.pop_back();
		return true;
	}

	std::string Next() {
		if (postion + 1 > postion_max) return "";
		++postion;
		current = _tokens[postion];
		return _tokens[postion];
	}
	void Skip() {
		if (postion < postion_max) {
			postion++;
			current = _tokens[postion];
		}
	}
	std::string SeekNext() {
		if (postion + 1 > postion_max) return "";
		return _tokens[postion + 1];
	}

	std::string Prev() {
		if (postion == 0) return _tokens[0];
		return _tokens[postion - 1];
	}

	void Back() {
		postion--;
		current = _tokens[postion];
	}

	std::string Current() {
		return _tokens[postion];
	}

	bool IsEnd() {
		return (postion >= postion_max);
	}

	bool Continue() {
		postion++;
		current = _tokens[postion];
		if (postion > postion_max) postion = postion_max;
		return (postion == postion_max);
	}
};

bool GetValues(TokenCompiller* tc, Object* obj, std::string type);

bool GetFunction(TokenCompiller* tc, Object* obj, function* func) {
	WriteCommand(Command::FUNCTION);
	int args = (short int)func->f_arguments.size();
	WriteBit((short int)func->index);
	WriteBit((short int)args);
	if (tc->Next() != "(") {
		Error("bad token '(' needed but " + tc->Current() + " given",10);
		return false;
	}
	tc->Skip();
	if (args > 0) {
		for (int i = 0; i < args; i++) {
			std::string n_type = func->f_arguments[i].Type;
			bool ret = GetValues(tc, obj, n_type);
			tc->Skip();
			if (tc->Current() == ",") tc->Skip();
			if (ret == false) return false;
		}
	}
	if (tc->Current() != ")") {
		Error("bad token ')' needed but " + tc->Current() + " given",11);
		return false;
	}
	//tc->Skip();
	return true;
}

bool GetValues(TokenCompiller* tc, Object* obj, std::string type) {
	std::string g_varible = tc->Current();
	function* t_fun = fWrapper::GetFunction(g_varible);
	if (t_fun != nullptr) {
		if (t_fun->f_return.Type == type) {
			return GetFunction(tc, obj, t_fun);
		}
		else {
			Error("varible type missed, " + type + " expected but " + t_fun->f_return.Type + " is here (from function - "+t_fun->f_name+")",12);
			return false;
		}
	}
	varible* t_var = obj->FindLocal(g_varible);
	if (t_var != nullptr) { // zmienna
		if (t_var->Type == type) {
			WriteCommand(Command::LOCAL_VARIBLE);
			//WriteBit(getVaribleIndex(t_var->Type));
			WriteValue(t_var->Type, t_var->index);
			return true;
		}
		else {
			Error("varible type missed, " + type + " expected but " + t_var->Type + " is here",13);
			return false;
		}
	}
	else {
		// wartoœæ
		if (g_varible == "null") {
			WriteCommand(Command::NULL_VALUE);
			return true;
		}
		else {
			if (TryToParse(g_varible, type)) {
				WriteCommand(Command::VALUE);
				WriteBit(getVaribleIndex(type));
				WriteString(g_varible);
				return true;
			}
			else {
				Error("unknown varible '"+ g_varible +"' type '"+type+"' expected",14);
				return false;
			}
		}
	}
	return true;
}
inline bool file_exists(const std::string& name) {
	std::ifstream f(name.c_str());
	return f.good();
}

const int VERSION_MAIN = 1;
const int VERSION_SUB = 51;
/*
int exit_error() {
	int a = 0;
	return 10 / a;
}
#undef EXIT_FAILURE
#define EXIT_FAILURE exit_error();
*/

int main(int argc, char** argv) {
	std::cout << "ArtCore Compiler " << VERSION_MAIN << "." << VERSION_SUB << std::endl;
	fWrapper::Init();
	oWrapper::Init();
	std::string output = "";
	bool atLeastOneLibIsLoaded = false;
	bool atLeastOneObjectisLoaded = false;
	bool atLestOutputIsLoaded = false;

	if (argc < 2) {
		std::cout << "more args is needed" << std::endl;
		//return EXIT_FAILURE;

	}

	for (int i = 1; i < argc; i++) {
		std::string mode = argv[i];
		if (mode.back() == ' ') {
			mode = mode.substr(0, mode.length() - 2);
		}
		if (mode == "-version") {
			std::cout << VERSION_MAIN << "." << VERSION_SUB << std::endl;
		}
		if (mode == "-debug") {
			output = "D:\\projekt\\object_compile.acp";
			fWrapper::AddLib("D:\\projekt\\AScript.lib");
			oWrapper::CreateObject("C:\\Users\\atrox\\Desktop\\SpaceKompakt\\object\\o_player\\main.asc");
			oWrapper::CreateObject("C:\\Users\\atrox\\Desktop\\SpaceKompakt\\object\\o_enemy\\main.asc");
			oWrapper::CreateObject("C:\\Users\\atrox\\Desktop\\SpaceKompakt\\object\\o_bullet\\main.asc");
			oWrapper::CreateObject("C:\\Users\\atrox\\Desktop\\SpaceKompakt\\object\\o_enemy_bullet\\main.asc");
			atLeastOneLibIsLoaded = true;
			atLeastOneObjectisLoaded = true;
			atLestOutputIsLoaded = true;
			break;
		}
		if (mode == "-lib") {
			std::string argument = argv[i + 1];
			if (file_exists(argument)) {
				if (argument[0] == '"') {
					argument = argument.substr(1, argument.length() - 2);
				}
				if (fWrapper::AddLib(argument)) {
					atLeastOneLibIsLoaded = true;
					std::cout << "Lib loaded: " << argument << std::endl;
				}
				else {
					return EXIT_FAILURE;
				}
			}
			else {
				std::cout << "File '" << argument << "' not exists!" << std::endl;
			}
			i++;
		}
		if (mode == "-obj") {
			std::string argument = argv[i + 1];
			if (file_exists(argument)) {
				if (argument[0] == '"') {
					argument = argument.substr(1, argument.length() - 2);
				}
				if (oWrapper::CreateObject(argument)) {
					atLeastOneObjectisLoaded = true;
					std::cout << "Obj loaded: " << argument << std::endl;
				}
				else {
					return EXIT_FAILURE;
				}
			}
			else {
				std::cout << "File '" << argument << "' not exists!" << std::endl;
			}
			i++;
		}
		if (mode == "-output") {
			std::string argument = argv[i + 1];
			if (argument[0] == '"') {
				argument = argument.substr(1, argument.length() - 2);
			}
			output = argument;
			atLestOutputIsLoaded = true;
			std::cout << "Output set: " << argument << std::endl;
			i++;
		}

	}
	std::vector<std::string> ExcludeFromCompare = std::vector<std::string>({
		"point","rectangle","color"
		});
	OutputCode = std::vector<unsigned char>();

	if (!atLeastOneLibIsLoaded) {
		std::cout << "Error: no lib added" << std::endl;
		return EXIT_FAILURE;
	}
	if( !atLeastOneObjectisLoaded ) {
		std::cout << "Error: no object added" << std::endl;
		return EXIT_FAILURE;
	}
	if( !atLestOutputIsLoaded ) {
		std::cout << "Error: no output added" << std::endl;
		return EXIT_FAILURE;
	}
	// compiller
	/*
	WriteCommand(Command::TYPEDEF);
	for (int i = 0; i < ARRAY_SIZE(varible_type); i++) {
		WriteString(varible_type[i]);
	}
	WriteCommand(Command::END);
	*/

	// first bajt
	WriteBit('A');
	WriteBit('C');
	WriteBit(VERSION_MAIN);
	WriteBit(VERSION_SUB);
	// unused
	WriteBit('\0');
	WriteBit('\0');
	WriteBit('\0');
	WriteBit('\0');

	for (Object* obj : oWrapper::GetObjects()) {
		WriteCommand(Command::OBJECT_DEFINITION);
		WriteString(obj->Name);
		for (varible& local : obj->GetLocals()) {
			WriteCommand(Command::LOCAL_VARIBLE_DEFINITION);
			WriteBit(getVaribleIndex(local.Type));
			WriteBit(local.index);
			WriteString(local.Name);
			WriteBit(local.ReadOnly);
		}
		WriteCommand(Command::END);

		for (auto& fun : obj->Functions) {
			WriteCommand(Command::FUNCTION_DEFINITION);
			WriteString(fun.first);
			cObject = obj->Name;
			cFunc = fun.first;
			TokenCompiller tc(fun.second);
			tc.EnterNextScope();
			while (!tc.IsEnd()) {
				cLine = tc.GetLine();
				cLineCode = tc.GetCode(cLine);
				std::string token = tc.Next();
				if (token == "") {
					tc.Continue();
					continue;
				}
				// other
				{
					if (token == "other") {
						std::string objName = "";
						if (tc.Next() == "(") {
							objName = tc.Next();
						}
						else {
							Error("bad token - '(' need but '" + tc.Current() + "'",16); return EXIT_FAILURE;
						}
						Object* ref = oWrapper::GetObjectByName(objName);
						if (ref == nullptr) {
							Error("object not found - '" + objName + "'",17); return EXIT_FAILURE;
						}
						if (tc.Next() != ")") { Error("bad token - ')' need but '" + tc.Current() + "'",18); return EXIT_FAILURE; }
						if (tc.Next() != ".") { Error("bad token - '.' need but '" + tc.Current() + "'",19); return EXIT_FAILURE; }

						std::string searched_varible = tc.Next();
						varible* var = ref->FindLocal(searched_varible);

						if (var == nullptr) { Error("varible '" + searched_varible + "' not found in '" + objName + "' object",20); return EXIT_FAILURE; }

						WriteCommand(Command::OTHER);
						// instance type
						WriteBit(ref->CodeId);
						// varible type + index
						WriteValue(var->Type, var->index);
						
						if (!isValidOperator(tc.Next())) { Error("operator expected but  '" + tc.Current() + "' found",21); return EXIT_FAILURE; }
						WriteBit(getOperatorIndex(tc.Current()));

						// skip operator
						tc.Skip();

						GetValues(&tc, obj, var->Type);
						continue;

					}
					
				}
				// zmienna
				{
					varible* var = obj->FindLocal(token);
					if (var != nullptr) {
						if (isValidOperator(tc.Next())) { // zmiana zmiennej
							WriteCommand(Command::SET);
							WriteBit(getOperatorIndex(tc.Current()));
							WriteValue(var->Type, var->index);
							tc.Skip();
							if (!GetValues(&tc, obj, var->Type)) {
								return EXIT_FAILURE;
							}
							continue;
						}
						else {
							Error("bad operator '"+ tc.Current() +"'",28);
							return EXIT_FAILURE;
						}
					}
				}
				// funkcja
				{
					function* fun = fWrapper::GetFunction(token);
					if (fun != nullptr) {
						if (!GetFunction(&tc, obj, fun)) {
							return EXIT_FAILURE;
						}
						continue;
					}
				}
				// if
				{
					if (token == "if") {
						WriteCommand(Command::IF_TEST);
						if (tc.Next() != "("){
							Error("bad token '(' needed but " + tc.Current() + " given",22);
							return EXIT_FAILURE;
						}
						//tc.Skip();
						std::string comp_type = "undefined";
						bool have_operator = false;
						while (tc.SeekNext() != ")") {
							token = tc.Next();
							// other
							{
								if (token == "other") {
									std::string objName = "";
									if (tc.Next() == "(") {
										objName = tc.Next();
									}
									else {
										Error("bad token - '(' need but '" + tc.Current() + "'",23); return EXIT_FAILURE;
									}
									Object* ref = oWrapper::GetObjectByName(objName);
									if (ref == nullptr) {
										Error("object not found - '" + objName + "'",24); return EXIT_FAILURE;
									}
									if (tc.Next() != ")") { 
										Error("bad token - ')' need but '" + tc.Current() + "'", 25); return EXIT_FAILURE; }
									if (tc.Next() != ".") { 
										Error("bad token - '.' need but '" + tc.Current() + "'", 26); return EXIT_FAILURE; }

									std::string searched_varible = tc.Next();
									varible* var = ref->FindLocal(searched_varible);
									if (var != nullptr) {
										WriteCommand(Command::OTHER);
										WriteBit(ref->CodeId);
										WriteValue(var->Type, var->index);
										comp_type = var->Type;
										token = tc.Next();
									}
									else {
										Error("varible '" + searched_varible + "' not found in '" + objName + "' object",27); return EXIT_FAILURE;
									}
								}
							}
							{
								varible* var = obj->FindLocal(token);
								if (var != nullptr) {
									WriteCommand(Command::LOCAL_VARIBLE);
									WriteValue(var->Type, var->index);
									comp_type = var->Type;
									continue;
								}
							}
							{
								function* fun = fWrapper::GetFunction(token);
								if (fun != nullptr) {
									if (!GetFunction(&tc, obj, fun)) {
										return EXIT_FAILURE;
									}
									comp_type = fun->f_return.Type;
									continue;
								}
							}
							{
								if (isValidOperator2(token)) {
									if (have_operator) {
										Error("Bad operator, seccond given",30);
										return EXIT_FAILURE;
									}

									if ((std::find(ExcludeFromCompare.begin(), ExcludeFromCompare.end(), comp_type) != ExcludeFromCompare.end())) {
										Error("value type: '" + comp_type + "' cannot be compare",29);
										return EXIT_FAILURE;
									}

									have_operator = true;
									WriteCommand(Command::OPERATOR);
									WriteBit(getOperator2Index(token));
									continue;
								}
							}
							{
								if (comp_type == "undefined") {
									Error("Compilation error, first type to compare must be function or varible",31);
									return EXIT_FAILURE;
								}
								if (TryToParse(token, comp_type)) {
									WriteCommand(Command::VALUE);
									WriteBit(getVaribleIndex(comp_type));
									WriteString(token);
									continue;
								}
								else {
									Error("Wrong type to compare as seeccond",32);
									return EXIT_FAILURE;
								}
							}
							Error("Unexpected token " + token,34);
						}
						if (tc.Next() != ")") {
							Error("bad token ')' needed but " + tc.Current() + " given",33);
							return EXIT_FAILURE;
						}
						WriteCommand(Command::IF_BODY);
						tc.EnterNextScope();
						continue;
					}
					if (token == "end") {
						if (!tc.ExitScope()) {
							Error("unexpected 'end'",35);
							return EXIT_FAILURE;
						}
						continue;
					}
				}
				Error("Unexpected token " + token,36);
			}
			WriteCommand(Command::END);
			tc.ExitScope();

		}
		WriteCommand(Command::END);
		std::cout << obj->Name << "<<" << std::endl;
	}
	WriteCommand(Command::END);

	std::cout << "Compilation complite" << std::endl;

	std::ofstream outFile(output, std::ios_base::binary);
	// the important part
	for (const auto& e : OutputCode) {
		outFile << e;
	}
	outFile.close();
	std::cout << "Created " << output << std::endl;
	return EXIT_SUCCESS;
}