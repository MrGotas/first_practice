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

bool correctCond(StrArray& condArr, string tableName);
void delCom(string userCommand, string baseName);

#endif // DELETE