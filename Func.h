#pragma once
#include <string>
#include <vector>
#include <codecvt>
#include <vector>

const std::vector<std::string> Explode(std::string& String, char Delim);
const std::vector<std::string> Split(const std::string& String, char Delim);
bool IsComment(std::string line);
std::vector<std::string> MakeTokens(std::string& line, bool skip_semicolon = false);
void Error2(const std::string& message, int error_nr);
#define Error(msg, x) Error2(msg, __LINE__)
