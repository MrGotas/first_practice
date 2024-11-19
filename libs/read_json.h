#ifndef READ_JSON
#define READ_JSON

#include <iostream>
#include <fstream>
#include <filesystem>
#include "array.h"
#include <nlohmann/json.hpp>

using namespace std;
namespace fs = filesystem;
using json = nlohmann::json;

void readJson(Array* massTabl, string& baseName);
void createСsv(const string& csvPath, const json& columns);
string remQuotes(string column);

#endif // READ_JSON