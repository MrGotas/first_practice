#ifndef INSERT
#define INSERT

#include <iostream>
#include <fstream>
#include <filesystem>
#include "array.h"
#include <nlohmann/json.hpp>

using namespace std;
namespace fs = filesystem;
using json = nlohmann::json;

string remQuotes(string column);
int countWords (string words);
bool checkSyntax(string& tableName, string& values, string baseName, string userCommand, string command);
bool checkTable(string tablePath);
int countCol(string tableName);
bool checkValues (StrArray& valuesToCol);
bool isLock(string tablePath, string tableName);
void lockTable (string tablePath, string tableName);
void unlockTable (string tablePath, string tableName);
bool fileAvail (string nextCsv, string tablePath);
void checkCsv (string tablePath, string tableName, string& csv);
bool correctCond (StrArray& condArr);

#endif // INSERT