#ifndef INSTRUMENTS
#define INSTRUMENTS

#include <iostream>
#include <fstream>
#include <filesystem>
#include <nlohmann/json.hpp>
#include "array.h"

using namespace std;
namespace fs = filesystem;
using json = nlohmann::json;

int countWords (string words);
bool checkSyntax(string& tableName, string& values, string baseName, string userCommand, string command);
bool checkTable(string tablePath);
bool isLock(string tablePath, string tableName);
void lockTable (string tablePath, string tableName);
void unlockTable (string tablePath, string tableName);

#endif // INSTRUMENTS