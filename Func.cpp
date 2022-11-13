#include "Func.h"
#include "Members.h"

extern int cLine;
extern std::string cFunc;
extern std::string cObject;
extern std::string cLineCode;

void Error(std::string message) {
	std::cout << "Error at line: '" << cLine << "' in Object: '" << cObject << "' Function: '" << cFunc << "' - Message: " << message << std::endl;
	std::cout << cLineCode << std::endl;

}
const std::vector<std::string> Explode(std::string& String, const char Delim)
{
	std::vector<std::string> tokens;
	size_t prev = 0, pos = 0;
	do
	{
		pos = String.find(Delim, prev);
		if (pos == std::string::npos) pos = String.length();
		std::string token = String.substr(prev, pos - prev);
		if (!token.empty()) tokens.push_back(token);
		prev = pos + 1;
	} while (pos < String.length() && prev < String.length());
	return tokens;
}

const std::vector<std::string> Split(std::string& String, const char Delim)
{
	std::vector<std::string> internal;
	std::stringstream ss(String);
	std::string tok;

	while (getline(ss, tok, Delim)) {
		if (!tok.empty())
			internal.push_back(tok);
	}

	return internal;
}

bool IsComment(std::string line) {
	line.erase(remove(line.begin(), line.end(), ' '), line.end());
	line.erase(remove(line.begin(), line.end(), '\t'), line.end());
	return (line.substr(0, 2) == "//");
}

std::vector<std::string> MakeTokens(std::string& line, bool SkipSemicolon) {
	std::vector<std::string> tokens = std::vector<std::string>();
	if (line.length() == 0) return tokens;

	std::string c_token = "";
	bool is_quote = false;

	for (int i = 0; i < line.length(); i++) {
		if (SkipSemicolon) {
			if (line[i] == ';') {
				i = (int)line.length();
				continue;
			}
		}
		// string z aktualnego znaku
		std::string lineS (1, line[i]);
		// spacje i taby
		if (line[i] == ' ' || line[i] == '\t') {
			if (is_quote) {
				c_token += line[i];
				continue;
			}
			else {
				if (c_token.length() > 0) {
					tokens.push_back(c_token);
					c_token = "";
				}
			}

		}// liczbowe
		else if ( (i > 0 && (line[i - 1] == ' ' && ((line[i] >= '0' && line[i] <= '9') || line[i] == '-')))) {
			std::string number = "";
			number += line[i];
			while (true && i+1 < line.length()) {
				char next_char = line[i + 1];
				if (next_char >= '0' && next_char <= '9' || next_char == 'f' || next_char == '.' ) {
					number += next_char;
					i++;
					if (next_char == 'f') break;
				}
				else {
					break;
				}
			}
			tokens.push_back(number);
		}// apostrofy do stringów
		else if (line[i] == '\"') {
			is_quote = !is_quote;
		}// funkcyjne
		else if (line[i] == '.' || line[i] == ',' || line[i] == '(' || line[i] == ')' || line[i] == '[' || line[i] == ']') {
			if (is_quote) {
				c_token += line[i];
				continue;
			}
			if (c_token.length() > 0) {
				tokens.push_back(c_token);
				c_token = "";
			}
			tokens.push_back(lineS);
		}// matematyczne
		else if (isValidOperator(lineS)) {
			if (is_quote) {
				c_token += line[i];
				continue;
			}
			if (c_token.length() > 0) {
				tokens.push_back(c_token);
				c_token = "";
			}
			tokens.push_back(lineS);
		}// logiczne
		else {
			std::string t_tk = "" + line[i] + line[i + 1];
			if (isValidOperator2(t_tk)) {
				if (is_quote) {
					c_token += line[i];
					continue;
				}
				if (c_token.length() > 0) {
					tokens.push_back(t_tk);
					c_token = "";
					i++;
				}
				tokens.push_back(lineS);
				i++;
			}
			else {
				c_token += line[i];
				if (i == line.length() - 1) {
					tokens.push_back(c_token);
				}
			}
		}
	}

	return tokens;
}


template<typename T>
inline bool VectorContain(std::vector<T>& vector, T element)
{
	if (vector.size() == 0) return -1;
	return (std::find(vector.begin(), vector.end(), element) != vector.end());
}
