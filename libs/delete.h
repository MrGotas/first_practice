#ifndef DELETE
#define DELETE

#include <iostream>
#include <fstream>
#include <filesystem>
#include <nlohmann/json.hpp>
#include "array.h"
#include "instruments.h"

using namespace std;
using json = nlohmann::json;

bool checkCond (StrArray& tokens, string line);
void addToTemp (string line, string tempPath, string firstLine);
bool checkRes (string result);
bool checkVals (string line, string condVal, int valuePos);
string withoutApostr (string word);
void replaceTok(StrArray& tokens, string nextToken);
void toTokens(StrArray& condArr, StrArray& tokens);
bool correctCond(StrArray& condArr, string tableName);
void delCom(string userCommand, string baseName);

#endif // DELETE