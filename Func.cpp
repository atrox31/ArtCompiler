#include "Func.h"

#include <iostream>
#include <sstream>

#include "Members.h"

extern int c_line;
extern std::string c_func;
extern std::string c_object;
extern std::string c_line_code;

void Error2(const std::string& message, const int error_nr) {
	std::cout << "Error at line: '" << c_line << "' in Object: '" << c_object << "' Function: '" << c_func << "' - Message: " << message << "[" << error_nr << "]" << std::endl;
	std::cout << c_line_code << std::endl;
std::exit(error_nr);
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

const std::vector<std::string> Split(const std::string& String, const char Delim)
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

std::vector<std::string> MakeTokens(std::string& line, bool skip_semicolon) {
	std::vector<std::string> tokens = std::vector<std::string>();
	if (line.length() == 0) {
		// empty token for correct line number if comment
		tokens.emplace_back("");
		return tokens;
	}

	std::string c_token;
	bool is_quote = false;

	for (int i = 0; i < line.length(); i++) {
		// previous char
		const char p_char = i > 0 ? line[i - 1] : '\0';
		// current char
		const char c_char = line[i];
		// next char
		const char n_char = i < line.length() - 1 ? line[i + 1] : '\0';

		if(c_char == '/' && n_char == '/')
		{
			// comment in line
			if (!c_token.empty()) {
				tokens.emplace_back(c_token);
			}
			if(tokens.empty())
			{
				// insert empty token if not any
				tokens.emplace_back("");
			}
			return tokens;
		}
		// for ArtScript.lib, semicons are use to comment
		if (skip_semicolon) {
			if (c_char == ';') {
				i = static_cast<int>(line.length());
				continue;
			}
		}
		// string from actual char
		std::string line_s (1, c_char);
		// space and tabs
		if (c_char == ' ' || c_char == '\t'|| c_char == ';') {
			if (is_quote) {
				c_token += c_char;
				continue;
			}
			else {
				if (c_token.length() > 0) {
					tokens.emplace_back(c_token);
					c_token = "";
				}
			}

		}// numbers
		else if (
			(i > 0 && (	( p_char == ' ' || c_token.empty()) && ((c_char >= '0' && c_char <= '9') || (c_char == '-') && (c_char >= '0' && c_char <= '9')) ))) {
			std::string number;
			number += c_char;
			while (true && i+1 < line.length()) {
				char next_char = line[i + 1];
				if (next_char >= '0' && next_char <= '9' || next_char == 'f' || next_char == '.' ) {
					number += next_char;
					i++;
					if (next_char == 'f') break;
				}
				else {
					if (next_char == ':') {
						number += next_char;
					}
					break;
				}
			}
			tokens.emplace_back(number);
		}// string values
		else if (c_char == '\"') {
			c_token += c_char;
			is_quote = !is_quote;
		}// functions
		else if (c_char == '.' || c_char == ',' || c_char == '(' || c_char == ')' || c_char == '[' || c_char == ']') {
			if (is_quote) {
				c_token += c_char;
				continue;
			}
			if (c_token.length() > 0) {
				tokens.emplace_back(c_token);
				c_token = "";
			}
			tokens.emplace_back(line_s);
		}// math
		else if (isValidOperator(line_s)) {
			if (is_quote) {
				c_token += c_char;
				continue;
			}
			if (c_token.length() > 0) {
				tokens.emplace_back(c_token);
				c_token = "";
			}
			tokens.emplace_back(line_s);
		}// logic
		else {
			std::string t_tk = "" + c_char + line[i + 1];
			if (isValidOperator2(t_tk)) {
				if (is_quote) {
					c_token += c_char;
					continue;
				}
				if (c_token.length() > 0) {
					tokens.emplace_back(t_tk);
					c_token = "";
					i++;
				}
				tokens.emplace_back(line_s);
				i++;
			}
			else {
				c_token += c_char;
				if (static_cast<size_t>(i) == line.length() - 1) {
					tokens.emplace_back(c_token);
				}
			}
		}
	}

	return tokens;
}


template<typename T>
inline bool VectorContain(std::vector<T>& vector, T element)
{
	if (vector.size() == 0) return false;
	return (std::find(vector.begin(), vector.end(), element) != vector.end());
}
