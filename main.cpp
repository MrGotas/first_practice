#include <iostream>
#include <string>
#include <sstream>
#include "libs/read_json.h"
#include "libs/insert.h"
#include "libs/instruments.h"
#include "libs/array.h"

using namespace std;
namespace fs = filesystem;

void insertCom(string userCommand, string baseName);
void delCom(string userCommand, string baseName);

int main() {
    /*
    cout << "Напиши чего ты хочешь: ";
    string userWish;
    cin >> userWish;
    cout << "Если хочется  " << userWish << " - перехочется." << endl; 
    cout << "Сиди учись, пока ещё студент" << endl;
    */
    string baseName;
    readJson(baseName);

    string userCommand;
    while (true){
        getline(cin, userCommand);

        stringstream ss(userCommand);
        string temp;
        ss >> temp;

        if (temp == "EXIT"){
            return 0;
        }else if (temp == "INSERT"){
            insertCom(userCommand, baseName);
        }else if (temp == "DELETE"){
            delCom(userCommand, baseName);
        }else if (temp == "SELECT"){

        }else{
            cerr << "ERROR_1: Unknown command." << endl;
        }
    }

    return 0;
}

void delCom(string userCommand, string baseName){
    string tableName, conditions;
    if (!checkSyntax(tableName, conditions, baseName, userCommand, "DEL")){
        return;
    }

    string tablePath = baseName + "/" + tableName;
    if (!checkTable(tablePath)){
        cerr << "ERROR_11: Unknown table name." << endl;
        return;
    }

    stringstream ss (conditions);
    string temp;
    StrArray condArr;
    while (ss >> temp){
        condArr.push(temp);
    }

    if (!correctCond(condArr)){
        return;
    }

    /*
    if (countWords(userCommand) < 7){
        cerr << "ERROR_12: Unknown command." << countWords(userCommand) << endl;
        return;
    }

    stringstream ss (userCommand);
    string tableName, temp;
    ss >> temp; // DELETE

    ss >> temp;
    if (temp != "FROM"){
        cerr << "ERROR_13: Unknown command." << endl;
        return;
    }

    ss >> tableName;
    string tablePath = baseName + "/" + tableName;

    if (isLock(tablePath, tableName)){
        cout << "The table is currently locked for use, try again later." << endl;
        return;
    }else{
        lockTable(tablePath, tableName);
    }

    int countCol = 0;
    if (!checkTable(tablePath, countCol)){
        cerr << "ERROR_14: Unknown table name." << endl;
        unlockTable(tablePath, tableName);
        return;
    }

    ss >> temp;
    if (temp != "WHERE"){
        cerr << "ERROR_15: Unknown command." << endl;
        unlockTable(tablePath, tableName);
        return;
    }

    string operations;
    while (getline(ss, operations));

    StrArray tokens; // массив токенов
    if (!toTokens(operations, tokens, baseName, tableName)){
        unlockTable(tablePath, tableName);
        return;
    }

    for (int i = 0; i < tokens.sizeM(); i++){
        string res;
        tokens.get(i, res);
        cout << res << endl;
    }


    ifstream csv (tablePath + "/1.csv");
    ofstream tempFile(tablePath + "/temp.txt");
    string line;
    getline(csv, line);
    tempFile << line;

    int lines;
    while (getline(csv, line)){
        lines ++;

        if (isDel(tokens, line)){
            continue;
        }else{
            tempFile << endl << line;
        }
    }

    unlockTable(tablePath, tableName);
    */
}