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
void fromCsvToTemp (string baseName, string tableName, string column);
void createTemp(StrArray& outputCol, string baseName, bool wasWhere);
void massWithoutCol (StrArray& outputCol);
void crossJoin(StrArray& outputCol, string baseName, int countColumns, int nowTable, string& output, string& colNames, string& tempStr);
void removeComma(string& value);
bool checkOutputCol(string& tableCol, string baseName);
bool checkFrom (StrArray& fromTable, StrArray& outputCol);
void selectCom(string userCommand, string baseName);

#endif // SELECT