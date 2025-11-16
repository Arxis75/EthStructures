#pragma once

#include <sstream>
#include <vector>
#include <memory>

using std::string;
using std::stringstream;
using std::vector;

using std::unique_ptr;
using std::make_unique;
using std::make_pair;

vector<string> split(const string& list, const string& separator);

string removeCharsFromString(const string &val, const char* charsToRemove);