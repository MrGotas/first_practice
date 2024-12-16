#ifndef SELECT
#define SELECT

#include <iostream>
#include <fstream>
#include <filesystem>
#include <nlohmann/json.hpp>
#include "array.h"
#include "instruments.h"

using namespace std;
namespace fs = filesystem;
using json = nlohmann::json;

void lockTables(StrArray& tables, string baseName, string command);
bool isLockTables(StrArray& tables, string baseName);
string getValueFromLine(string line, int columnNum);
void fromCsvToTemp (string baseName, string tableName, string column, string& colNames);
void createTemp(StrArray& outputCol, string baseName, bool wasWhere, string& colNames);
void massWithoutCol (StrArray& outputCol);
void clearTemp(string& tempStr);
void crossJoin(string baseName, StrArray& outputCol, int nowTable, string& output, string tempStr);
void removeComma(string& value);
bool checkOutputCol(string& tableCol, string baseName);
bool checkFrom (StrArray& fromTable, StrArray& outputCol);
void selectCom(string userCommand, string baseName);

#endif // SELECT