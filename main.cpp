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
	FUNCTION,
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
			Error("try to parse " + value + " to " + type + " - error");
			return false;
		}
	}
	else if (type == "float") {
		try{
			std::ignore = std::stof(value);
		return true;
		}
			catch (...) {
			Error("try to parse " + value + " to " + type + " - error");
			return false;
		}
	}
	else if (type == "bool") {
		boost::algorithm::to_lower(value);
		if (value == "true" || value == "false") return true;
		Error("try to parse " + value + " to " + type + " - error");
		return false;
	}
	else if (type == "string") {
		
		return true;
	}
	Error("type must be primitive");
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
			line.push_back((int)_tokens.size()-1);
			_code.push_back(code);
		}
		postion = -1;
		postion_max = (int)_tokens.size() - 1;
	}

	std::string GetCode(int line) {
		if (line >= (int)_code.size()) line = (int)_code.size() - 1;
		return _code[line];
	}

	int GetLine() {
		for (int i = 0; i < line.size() - 1; i++) {
			if (postion >= line[i] && postion <= line[i + 1]) return line[i];
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
			Error("scope is too long! ("+std::to_string(skip)+") max 254");
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
	if (tc->Next() != "(") {
		Error("bad token '(' needed but " + tc->Current() + " given");
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
		Error("bad token ')' needed but " + tc->Current() + " given");
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
			Error("varible type missed, " + type + " expected but " + t_fun->f_return.Type + " is here (from function - "+t_fun->f_name+")");
			return false;
		}
	}
	varible* t_var = obj->FindLocal(g_varible);
	if (t_var != nullptr) { // zmienna
		if (t_var->Type == type) {
			WriteCommand(Command::LOCAL_VARIBLE);
			WriteBit(getVaribleIndex(t_var->Type));
			WriteValue(t_var->Type, t_var->index);
			return true;
		}
		else {
			Error("varible type missed, " + type + " expected but " + t_var->Type + " is here");
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
				Error("unknown varible '"+ g_varible +"' - '"+type+"' expected");
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

int main(int argc, char** argv) {
	std::cout << "ArtCore Compiler 1.0" << std::endl;
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
		if (mode == "-debug") {
			output = "D:\\projekt\\object_compile.acp";
			fWrapper::AddLib("D:\\projekt\\AScript.lib");
			oWrapper::CreateObject("C:\\Users\\atrox\\Desktop\\final_test_project\\object\\instance_w_sprite\\main.asc");
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

		WriteCommand(Command::FUNCTION_DEFINITION);
		for (auto& fun : obj->Functions) {
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
				// zmienna
				{
					varible* var = obj->FindLocal(token);
					if (var != nullptr) {
						if (tc.Next() == "=") { // zmiana zmiennej
							WriteCommand(Command::SET);
							WriteValue(var->Type, var->index);
							tc.Skip();
							GetValues(&tc, obj, var->Type);
							continue;
						}
						else {
							Error("bad token");
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
							Error("bad token '(' needed but " + tc.Current() + " given");
							return EXIT_FAILURE;
						}
						//tc.Skip();
						std::string comp_type = "undefined";
						bool have_operator = false;
						while (tc.SeekNext() != ")") {
							token = tc.Next();
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
										Error("Bad operator, seccond given");
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
									Error("Compilation error, first type to compare must be function or varible");
									return EXIT_FAILURE;
								}
								if (TryToParse(token, comp_type)) {
									WriteCommand(Command::VALUE);
									WriteBit(getVaribleIndex(comp_type));
									WriteString(token);
									continue;
								}
								else {
									Error("Wrong type to compare as seeccond");
									return EXIT_FAILURE;
								}
							}
							Error("Unexpected token " + token);
						}
						if (tc.Next() != ")") {
							Error("bad token ')' needed but " + tc.Current() + " given");
							return EXIT_FAILURE;
						}
						WriteCommand(Command::IF_BODY);
						tc.EnterNextScope();
						continue;
					}
					if (token == "end") {
						if (!tc.ExitScope()) {
							Error("unexpected 'end'");
							return EXIT_FAILURE;
						}
						continue;
					}
				}
				Error("Unexpected token " + token);
			}
			WriteCommand(Command::END);
			tc.ExitScope();

		}
		WriteCommand(Command::END);
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