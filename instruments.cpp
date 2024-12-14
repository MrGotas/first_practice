#include "libs/instruments.h"

int countWords (string words){
    stringstream ss (words);
    string word;
    int countWords = 0;

    while (ss >> word){
        countWords++;
    }

    return countWords;
}

bool checkSyntax(string& tableName, string& values, string baseName, string userCommand, string command){
    int countWor = countWords(userCommand); // проверка кол-ва слов
    if (command == "INS" && countWor < 5 || command == "DEL" && countWor < 7){
        cerr << "ERROR_2: Unknown command." << endl;
        return false;
    }

    stringstream ss (userCommand);
    string temp;
    ss >> temp; // первое слово

    ss >> temp;
    if (command == "INS" && temp != "INTO" || command == "DEL" && temp != "FROM"){
        cerr << "ERROR_3: Unknown command." << endl;
        return false;
    }

    ss >> tableName;

    ss >> temp;
    if (command == "INS" && temp != "VALUES" || command == "DEL" && temp != "WHERE"){
        cerr << "ERROR_4: Unknown command." << endl;
        return false;
    }

    while (ss >> temp){
        values += temp + " ";
    }

    return true;
}

bool checkTable(string tablePath){
    if (fs::exists(tablePath) && fs::is_directory(tablePath)){
        return true;
    }else{
        return false;
    }
}

bool isLock(string tablePath, string tableName){
    string path = tablePath + "/" + tableName + "_lock";
    ifstream file (path);
    if (!file.is_open()){
        cerr << "ERROR_8: Unable to open file: " << path << endl;
        return false;
    }

    int value;
    file >> value;
    file.close();

    if (value == 1){
        return true;
    }else if (value == 0){
        return false;
    }else{
        cerr << "ERROR_9: Unknown value from file: " << path << endl;
        return false;
    }
}

void lockTable (string tablePath, string tableName){
    string path = tablePath + "/" + tableName + "_lock";
    ofstream file (path);
    file << 1;
    file.close();
}

void unlockTable (string tablePath, string tableName){
    string path = tablePath + "/" + tableName + "_lock";
    ofstream file (path);
    file << 0;
    file.close();
}

bool columnExist (string tableName, string columnName){
    ifstream file("files/schema.json");
    json jsonData;
    file >> jsonData;
    file.close();

    if (jsonData.contains("structure") && jsonData["structure"].contains(tableName)){
        const auto& columns = jsonData["structure"][tableName];
        for (const auto& column : columns){
            if (column == columnName){
                return true;
            }
        }
    }
    return false;
}

int getTurplLim (){
    ifstream jsonFile("files/schema.json");
    json configs;
    jsonFile >> configs;
    jsonFile.close();

    return configs["tuples_limit"];
}

bool csvAvail (string csv){
    if (fs::exists(csv)){
        return true;
    }else{
        return false;
    }
}

void getTablCol (string tablCol, string& table, string& column){
    stringstream ss(tablCol);
    getline(ss, table, '.');
    getline(ss, column);
}

int colIndex (string table, string column){
    ifstream jsonFile("files/schema.json");
    json configs;
    jsonFile >> configs;
    jsonFile.close();

    const auto& tableColumns = configs["structure"][table];
    int columnPos = 1; // чтобы пропустить table_pk
    for (const auto& tableColumn : tableColumns){ //нахоидм до какого значения строки идти
        columnPos++;
        if (tableColumn == column){
            break;
        }
    }
    return columnPos;
}