#include <iostream>
#include <string>
#include <sstream>
#include "libs/instruments.h"
#include "libs/read_json.h"
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

void insertCom(string userCommand, string baseName){
    if (countWords(userCommand) < 5){
        cerr << "ERROR_2: Unknown command." << countWords(userCommand) << endl;
        return;
    }

    stringstream ss (userCommand);
    string tableName, temp;
    ss >> temp; // INSERT
    
    ss >> temp;
    if (temp != "INTO"){
        cerr << "ERROR_3: Unknown command." << endl;
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
        cerr << "ERROR_4: Unknown table name." << endl;
        unlockTable(tablePath, tableName);
        return;
    }

    ss >> temp;
    if (temp != "VALUES"){
        cerr << "ERROR_5: Unknown command." << endl;
        unlockTable(tablePath, tableName);
        return;
    }

    StrArray valuestoCol;
    while (ss >> temp){
        countCol--;
        valuestoCol.push(temp);
    }
    
    if (countCol != 0){
        if (countCol < 0){
            cerr << "ERROR_8: Many argumenst to insert." << endl;
            unlockTable(tablePath, tableName);
            return;
        }else{
            cerr << "ERROR_9: Few argumenst to insert." << endl;
            unlockTable(tablePath, tableName);
            return;
        }
        unlockTable(tablePath, tableName);
        return;
    }
    if (checkValues(valuestoCol) == false){
        cerr << "ERROR_10: Incorrect values." << endl;
        unlockTable(tablePath, tableName);
        return;
    }

    string pkFile = tablePath + "/" + tableName + "_pk";
    ifstream filePk(pkFile);
    if (!filePk.is_open()){
        cerr << "ERROR_11: Unable to open file: " << pkFile << endl;
        unlockTable(tablePath, tableName);
        return;
    }
    int pkVal;
    filePk >> pkVal;
    filePk.close();

    // сделать проверку на переполнение
    string dataToTable;
    dataToTable = to_string(pkVal) + ";";
    for (size_t i = 0; i < valuestoCol.sizeM(); i++){
        string elem;
        valuestoCol.get(i, elem);
        dataToTable += elem + ";";
    }

    string csv;
    checkCsv (tablePath, tableName, csv);

    ofstream fileCsv(tablePath + "/" + csv, ios::app);
    fileCsv << endl << dataToTable;
    fileCsv.close();

    ofstream filePkEnd(pkFile);
    filePkEnd << pkVal + 1;

    unlockTable(tablePath, tableName);
}

void delCom(string userCommand, string baseName){
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

    /*for (int i = 0; i < tokens.sizeM(); i++){
        string res;
        tokens.get(i, res);
        cout << res << endl;
    }*/


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
}