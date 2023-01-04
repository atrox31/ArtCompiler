#include <string>
#include <vector>
#include <map>
#include <fstream>
#include <iomanip>
#include <iostream>
#include <vector>
#include <fstream>
#include<algorithm>
#include <boost/algorithm/string.hpp>
#include <boost/lexical_cast.hpp>

#include "main.h"
#include "Members.h"
#include "Func.h"
#include "fWrapper.h"
#include "Object.h"
#include "oWrapper.h"

int c_line = 0;
std::string c_func = "none";
std::string c_object = "none";
std::string c_line_code = "none";
std::vector<unsigned char> OutputCode;

#define ALLOW_0_LENGTH_STRING true


template <typename E>
constexpr typename std::underlying_type<E>::type ToUnderlying(E e) noexcept {
	return static_cast<typename std::underlying_type<E>::type>(e);
}

enum class Command {
	// definicje
	OBJECT_DEFINITION,
	FUNCTION_DEFINITION,
	LOCAL_variable_DEFINITION,
	TYPEDEF,
	// odwo�ania
	OBJECT,
	OBJECT_variable,
	LOCAL_variable,
	VALUE,NULL_VALUE,
	FUNCTION,OTHER,
	// operatory
	SET,
	OPERATOR,
	// polecenia
	TYPE, IF_BODY, IF_TEST, ELSE,
	END,
};


void Uint32To2ByteChar(const unsigned int input, unsigned char* output)
{
	output[0] = (input >> 8) & 0xFF;
	output[1] = input & 0xFF;
}
void ByteCharToUint32(const unsigned char* input, unsigned int* output)
{
	*output = (input[0] << 8) | (input[1] << 0);
}


bool TryToParse(std::string value, const std::string& type) {
	if (value == "nul") {
		return true;
	}else
	if (type == "int") {
		try {
			std::ignore = std::stoi(value);
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

void WriteString(const std::string& text) {
	if (text.length() == 0) return;
	for (const char i : text)
	{
		if((unsigned char)i != '\"')
		OutputCode.push_back((unsigned char)i);
	}
	OutputCode.push_back('\1');
}

void WriteBit(short int value) {
	OutputCode.push_back((unsigned char)value);
}

void WriteCommand(Command value) {
	OutputCode.push_back((unsigned char)ToUnderlying(value));
}

void WriteValue(const std::string& type, short int value) {
	if (type.length() == 0) return;
	short int _type = getVariableIndex(type);
	OutputCode.push_back((unsigned char)_type);
	OutputCode.push_back((unsigned char)value);
}


class TokenCompiler {

	std::vector<std::string> _tokens;
	std::vector<std::string> _code;
	int _position;
	int _position_max;
	std::vector<int> _line;
	std::vector<int> _skip_ptr;
	std::string _current;

public:
	explicit TokenCompiler(std::string code) {
		_line = std::vector<int>();
		_skip_ptr = std::vector<int>();
		_tokens = std::vector<std::string>();
		_code = std::vector<std::string>();
		for (std::string code : Explode(code, '\n')) {
			std::vector<std::string> tokens = MakeTokens(code);
			_tokens.insert(_tokens.end(), tokens.begin(), tokens.end());
			_line.push_back((int)tokens.size() - 1);
			_code.push_back(code);
		}
		_position = -1;
		_position_max = (int)_tokens.size() - 1;
	}

	std::string GetCode(const int i) {
		return _code[i];
	}

	int GetLine() const
	{
		int cline = 0;
		int cpos = 0;
		for (int i = 0; i < _line.size() - 1; i++) {
			cpos += _line[i];
			cline++;
			if (cpos >= _position) {
				return cline;
			}
		}
		return 0;
	}

	void EnterNextScope() {
		// write 2 values to convert later to 16bit
		WriteBit(0);
		WriteBit(0);
		
		_skip_ptr.push_back((int)OutputCode.size() - 1);
	}
	bool ExitScope() {
		const unsigned int skip = ((int)OutputCode.size() - 1) - _skip_ptr.back();
		if (skip > 0xFFFF) {
			Error("scope is too long! ("+std::to_string(skip)+") max 65535",9);
			return false;
		}
		unsigned char skip_bite[2];
		Uint32To2ByteChar(skip, skip_bite);

		OutputCode[_skip_ptr.back()] = skip_bite[0];
		OutputCode[_skip_ptr.back()+1] = skip_bite[1];
		_skip_ptr.pop_back();
		return true;
	}

	std::string Next() {
		if (_position + 1 > _position_max) return "";
		++_position;
		_current = _tokens[_position];
		return _tokens[_position];
	}
	void Skip() {
		if (_position < _position_max) {
			_position++;
			_current = _tokens[_position];
		}
	}
	std::string SeekNext() {
		if (_position + 1 > _position_max) return "";
		return _tokens[_position + 1];
	}

	std::string Prev() {
		if (_position == 0) return _tokens[0];
		return _tokens[_position - 1];
	}

	void Back() {
		_position--;
		_current = _tokens[_position];
	}

	std::string Current() {
		return _tokens[_position];
	}

	[[nodiscard]] bool IsEnd() const
	{
		return (_position >= _position_max);
	}

	bool Continue() {
		_position++;
		_current = _tokens[_position];
		if (_position > _position_max) _position = _position_max;
		return (_position == _position_max);
	}
};

bool GetValues(TokenCompiler* tc, Object* obj, const std::string& type);

bool GetFunction(TokenCompiler* tc, Object* obj, function* func) {
	WriteCommand(Command::FUNCTION);
	const int args = (short int)func->f_arguments.size();
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
			const bool ret = GetValues(tc, obj, n_type);
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

bool GetValues(TokenCompiler* tc, Object* obj, const std::string& type) {
	const std::string g_variable = tc->Current();
	if (function* t_fun = fWrapper::GetFunction(g_variable); t_fun != nullptr) {
		if (t_fun->f_return.Type == type) {
			return GetFunction(tc, obj, t_fun);
		}
		else {
			Error("variable type missed, " + type + " expected but " + t_fun->f_return.Type + " is here (from function - "+t_fun->f_name+")",12);
			return false;
		}
	}
	if (const variable* t_var = obj->FindLocal(g_variable); t_var != nullptr) { // variable
		if (t_var->Type == type) {
			WriteCommand(Command::LOCAL_variable);
			//WriteBit(getVariableIndex(t_var->Type));
			WriteValue(t_var->Type, t_var->index);
			return true;
		}
		else {
			Error("variable type missed, " + type + " expected but " + t_var->Type + " is here",13);
			return false;
		}
	}
	else {
		// value
		if (g_variable == "null") {
			WriteCommand(Command::NULL_VALUE);
			return true;
		}
		else {
			std::string token2 = g_variable;
			if (type == "point") {
				token2 += tc->Next();
			}
			if (TryToParse(token2, type)) {
				WriteCommand(Command::VALUE);
				WriteBit(getVariableIndex(type));
				WriteString(g_variable);
				return true;
			}
			else {
				Error("unknown variable '"+ g_variable +"' type '"+type+"' expected",14);
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

class Token
{
private:
	std::string _data_normal;
	std::string _data_lower;
public:
	explicit Token(std::string string)
	{
		_data_normal = string;
		std::transform(string.begin(), string.end(), string.begin(),
			[](const unsigned char c) { return std::tolower(c); });
		_data_lower = string;
	}
	Token(const Token& copy)
	{
		_data_normal = copy._data_normal;
		_data_lower = copy._data_lower;
	}
	Token operator+=(std::string string)
	{
		_data_normal += string;
		std::transform(string.begin(), string.end(), string.begin(),
			[](const unsigned char c) { return std::tolower(c); });
		_data_lower += string;
		return *this;
	}
	std::string GetNormal()
	{
		return _data_normal;
	}
	std::string GetLower()
	{
		return _data_lower;
	}

	[[nodiscard]] bool Empty() const
	{
		return _data_normal.empty();
	}

	void New(std::string string)
	{
		_data_normal = string;
		std::transform(string.begin(), string.end(), string.begin(),
			[](const unsigned char c) { return std::tolower(c); });
		_data_lower = string;
	}
};

#include <bitset>

std::ostream& operator<<(std::ostream& os, const std::bitset<32>& dt)
{
	for (int i=0; i<32; i+=8)
	{
		for (int j = i; j < i+8; j++)
		{
			os << dt[j];
		}
		os << " ";
	}
	return os;
}

int main(int argc, char** argv) {
	fWrapper::Init();
	oWrapper::Init();
	std::string output;
	bool at_least_one_lib_is_loaded = false;
	bool at_least_one_objects_loaded = false;
	bool at_lest_output_is_loaded = false;
	bool quiet = false;

	if (argc < 2) {
		std::cout << "more args is needed" << std::endl;
		//return EXIT_FAILURE;
	}

	for (int i = 1; i < argc; i++) {
		std::string mode = argv[i];
		if (mode.back() == ' ') {
			mode = mode.substr(0, mode.length() - 2);
		}
		if (mode == "-q")
		{
			quiet = true;
		}
		if (mode == "-version") {
			std::cout << VERSION_MAIN << '.' << VERSION_SUB << '.' << VERSION_PATH << std::endl;
			return EXIT_SUCCESS;
		}
		if (mode == "-lib") {
			std::string argument = argv[i + 1];
			if (file_exists(argument)) {
				if (argument[0] == '"') {
					argument = argument.substr(1, argument.length() - 2);
				}
				if (fWrapper::AddLib(argument)) {
					at_least_one_lib_is_loaded = true;
					if(!quiet) std::cout << "Lib loaded: " << argument << std::endl;
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
					at_least_one_objects_loaded = true;
					if (!quiet) std::cout << "Obj loaded: " << argument << std::endl;
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
			at_lest_output_is_loaded = true;
			if (!quiet) std::cout << "Output set: " << argument << std::endl;
			i++;
		}

	}
	std::vector<std::string> ExcludeFromCompare = std::vector<std::string>({
		"point","rectangle","color"
		});
	OutputCode = std::vector<unsigned char>();

	if (!at_least_one_lib_is_loaded) {
		std::cout << "Error: no lib added" << std::endl;
		return EXIT_FAILURE;
	}
	if( !at_least_one_objects_loaded ) {
		std::cout << "Error: no object added" << std::endl;
		return EXIT_FAILURE;
	}
	if( !at_lest_output_is_loaded ) {
		std::cout << "Error: no output added" << std::endl;
		return EXIT_FAILURE;
	}

	// first bajt
	WriteBit('A');
	WriteBit('C');
	WriteBit(VERSION_MAIN);
	WriteBit(VERSION_SUB);
	WriteBit(VERSION_PATH);
	// unused
	WriteBit('\0');
	WriteBit('\0');
	WriteBit('\0');

	for (Object* obj : oWrapper::GetObjects()) {
		WriteCommand(Command::OBJECT_DEFINITION);
		WriteString(obj->Name);
		for (variable& local : obj->GetLocals()) {
			WriteCommand(Command::LOCAL_variable_DEFINITION);
			WriteBit(getVariableIndex(local.Type));
			WriteString(local.Name);
		}
		WriteCommand(Command::END);

		for (auto& fun : obj->Functions) {
			WriteCommand(Command::FUNCTION_DEFINITION);
			WriteString(fun.first);
			c_object = obj->Name;
			c_func = fun.first;
			TokenCompiler tc(fun.second);
			tc.EnterNextScope();
			while (!tc.IsEnd()) {
				c_line = tc.GetLine();
				c_line_code = tc.GetCode(c_line);
				Token token(tc.Next());
				if (token.Empty()) {
					tc.Continue();
					continue;
				}
				// other
				{
					if (token.GetLower() == "other") {
						std::string obj_name;
						if (tc.Next() == "(") {
							obj_name = tc.Next();
						}
						else {
							Error("bad token - '(' need but '" + tc.Current() + "'",16); return EXIT_FAILURE;
						}
						Object* ref = oWrapper::GetObjectByName(obj_name);
						if (ref == nullptr) {
							Error("object not found - '" + obj_name + "'",17); return EXIT_FAILURE;
						}
						if (tc.Next() != ")") { Error("bad token - ')' need but '" + tc.Current() + "'",18); return EXIT_FAILURE; }
						if (tc.Next() != ".") { Error("bad token - '.' need but '" + tc.Current() + "'",19); return EXIT_FAILURE; }

						std::string searched_variable = tc.Next();
						variable* var = ref->FindLocal(searched_variable);

						if (var == nullptr) { Error("variable '" + searched_variable + "' not found in '" + obj_name + "' object",20); return EXIT_FAILURE; }

						WriteCommand(Command::OTHER);
						// instance type
						WriteBit(ref->CodeId);
						// variable type + index
						WriteValue(var->Type, var->index);
						
						if (!isValidOperator(tc.Next())) { Error("operator expected but  '" + tc.Current() + "' found",21); return EXIT_FAILURE; }
						WriteBit(getOperatorIndex(tc.Current()));

						// skip operator
						tc.Skip();

						GetValues(&tc, obj, var->Type);
						continue;

					}
					
				}
				// variable
				{
					variable* var = obj->FindLocal(token.GetNormal());
					if (var != nullptr) {
						if (isValidOperator(tc.Next())) { // change variable
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
				// function
				{
					function* fun = fWrapper::GetFunction(token.GetNormal());
					if (fun != nullptr) {
						if (!GetFunction(&tc, obj, fun)) {
							return EXIT_FAILURE;
						}
						continue;
					}
				}
				// if
				{
					if (token.GetLower() == "if") {
						WriteCommand(Command::IF_TEST);
						if (tc.Next() != "("){
							Error("bad token '(' needed but " + tc.Current() + " given",22);
							return EXIT_FAILURE;
						}
						//tc.Skip();
						std::string comp_type = "undefined";
						bool have_operator = false;
						while (tc.SeekNext() != ")") {
							token.New(tc.Next());
							// other
							{
								if (token.GetLower() == "other") {
									std::string obj_name;
									if (tc.Next() == "(") {
										obj_name = tc.Next();
									}
									else {
										Error("bad token - '(' need but '" + tc.Current() + "'",23); return EXIT_FAILURE;
									}
									Object* ref = oWrapper::GetObjectByName(obj_name);
									if (ref == nullptr) {
										Error("object not found - '" + obj_name + "'",24); return EXIT_FAILURE;
									}
									if (tc.Next() != ")") { 
										Error("bad token - ')' need but '" + tc.Current() + "'", 25); return EXIT_FAILURE; }
									if (tc.Next() != ".") { 
										Error("bad token - '.' need but '" + tc.Current() + "'", 26); return EXIT_FAILURE; }

									std::string searched_variable = tc.Next();
									variable* var = ref->FindLocal(searched_variable);
									if (var != nullptr) {
										WriteCommand(Command::OTHER);
										WriteBit(ref->CodeId);
										WriteValue(var->Type, var->index);
										comp_type = var->Type;
										token.New(tc.Next());
									}
									else {
										Error("variable '" + searched_variable + "' not found in '" + obj_name + "' object",27); return EXIT_FAILURE;
									}
								}
							}
							{
								variable* var = obj->FindLocal(token.GetNormal());
								if (var != nullptr) {
									WriteCommand(Command::LOCAL_variable);
									WriteValue(var->Type, var->index);
									comp_type = var->Type;
									continue;
								}
							}
							{
								function* fun = fWrapper::GetFunction(token.GetLower());
								if (fun != nullptr) {
									if (!GetFunction(&tc, obj, fun)) {
										return EXIT_FAILURE;
									}
									comp_type = fun->f_return.Type;
									continue;
								}
							}
							{
								if (isValidOperator2(token.GetNormal())) {
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
									WriteBit(getOperator2Index(token.GetNormal()));
									continue;
								}
							}
							{
								if (comp_type == "undefined") {
									Error("Compilation error, first type to compare must be function or variable", 31);
									return EXIT_FAILURE;
								}
								Token token2 = token;
								if (comp_type == "point"){
									token2 += tc.Next();
								}
								if (TryToParse(token2.GetNormal(), comp_type)) {
									WriteCommand(Command::VALUE);
									WriteBit(getVariableIndex(comp_type));
									WriteString(token2.GetNormal());
									continue;
								}
								else {
									Error("Wrong type to compare as seccond",32);
									return EXIT_FAILURE;
								}
							}
							Error("Unexpected token " + token.GetNormal(),34);
						}
						if (tc.Next() != ")") {
							Error("bad token ')' needed but " + tc.Current() + " given",33);
							return EXIT_FAILURE;
						}
						WriteCommand(Command::IF_BODY);
						WriteBit(getVariableIndex(comp_type));
						tc.EnterNextScope();
						continue;
					}
					if (token.GetLower() == "end") {
						if (!tc.ExitScope()) {
							Error("unexpected 'end'",35);
							return EXIT_FAILURE;
						}
						continue;
					}
					if (token.GetLower() == "else")
					{
						if (!tc.ExitScope()) {
							Error("unexpected 'else'", 36);
							return EXIT_FAILURE;
						}
						WriteCommand(Command::ELSE);
						tc.EnterNextScope();
						continue;
					}
				}
				Error("Unexpected token " + token.GetNormal(),37);
			}
			WriteCommand(Command::END);
			tc.ExitScope();

		}
		WriteCommand(Command::END);
		if (!quiet) std::cout << obj->Name << "<<" << std::endl;
	}
	WriteCommand(Command::END);

	if (!quiet) std::cout << "Compilation complete" << std::endl;

	std::ofstream out_file(output, std::ios_base::binary);
	// the important part
	for (const unsigned char& e : OutputCode) {
		out_file << e;
	}
	out_file.close();
	if (!quiet) std::cout << "Created " << output << std::endl;
	return EXIT_SUCCESS;
}
