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

const std::vector<std::string> Explode(std::string& String, const char Delim);
const std::vector<std::string> Split(std::string& String, const char Delim);
bool IsComment(std::string line);
std::vector<std::string> MakeTokens(std::string& line, bool SkipSemicolon = false);
void Error(std::string message);
template <typename T>
bool VectorContain(std::vector<T>& vector, T element);
