#pragma once
#include <string>
#include <vector>
#include <codecvt>
#include <vector>

const std::vector<std::string> Explode(std::string& String, const char Delim);
const std::vector<std::string> Split(std::string& String, const char Delim);
bool IsComment(std::string line);
std::vector<std::string> MakeTokens(std::string& line, bool skip_semicolon = false);
void Error2(const std::string& message, int error_nr);
template <typename T>
bool VectorContain(std::vector<T>& vector, T element);
#define Error(msg, x) Error2(msg, __LINE__);