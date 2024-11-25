#ifndef INSTRUMENTS
#define INSTRUMENTS

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
bool checkTable (string tableName, int& countCol);
bool isLock(string tablePath, string tableName);
void lockTable (string tablePath, string tableName);
void unlockTable (string tablePath, string tableName);
bool checkValues (StrArray& valuesToCol);
bool fileAvail (string nextCsv, string tablePath);
void checkCsv (string tablePath, string tableName, string& csv);

#endif // INSTRUMENTS