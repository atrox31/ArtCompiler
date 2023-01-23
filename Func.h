#pragma once
#include <string>
#include <vector>
#include <codecvt>
#include <vector>

std::vector<std::string> Explode(std::string& string, char separator);
std::vector<std::string> Split(const std::string& string, char separator);
bool IsComment(std::string line);
std::vector<std::string> MakeTokens(std::string& line, bool skip_semicolon = false);
void Error2(const std::string& message, int error_nr);
std::string ToLower(const std::string& string);
#define Error(msg, x) Error2(msg, __LINE__)
