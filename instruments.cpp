#include "libs/instruments.h"

string remQuotes(string column){
    string result;
    for (char symbol : column){
        if (symbol != '"'){
            result += symbol;
        }
    }
    return result;
}

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

int countCol(string tableName){
    ifstream file("files/schema.json");
    json jsonData;
    file >> jsonData;
    file.close();

    return jsonData["structure"][tableName].size();
}

bool checkValues (StrArray& valuesToCol){
    for (size_t i = 0; i < valuesToCol.sizeM(); i++){
        string temp;
        string correctVal; // то, куда будем записывать отредактированное значение
        if (i == 0){
            valuesToCol.get(i, temp);
            if (temp.size() < 5){
                return false;
            }
            for (size_t j = 0; j < temp.size(); j++){
                if (j == 0){
                    if (temp[j] != '('){
                        return false;
                    }else{
                        continue;
                    }
                }

                if (j == 1 || j + 2 == temp.size()){
                    if (temp[j] != '\''){
                        return false;
                    }else{
                        continue;
                    }
                }

                if (j + 1 == temp.size()){
                    if (temp[j] != ','){
                        return false;
                    }else{
                        continue;
                    }
                }
                correctVal += temp[j];
            }
        }else if (i + 1 == valuesToCol.sizeM()){
            valuesToCol.get(i, temp);
            if (temp.size() < 4){
                return false;
            }
            for (size_t j = 0; j < temp.size(); j++){
                if (j == 0 || j + 2 == temp.size()){
                    if (temp[j] != '\''){
                        return false;
                    }else{
                        continue;
                    }
                }
                if (j + 1 == temp.size()){
                    if (temp[j] != ')'){
                        return false;
                    }else{
                        continue;
                    }
                }
                correctVal += temp[j];
            }
        }else{
            valuesToCol.get(i, temp);
            if (temp.size() < 4){
                return false;
            }
            for (size_t j = 0; j < temp.size(); j++){
                if (j == 0 || j + 2 == temp.size()){
                    if (temp[j] != '\''){
                        return false;
                    }else{
                        continue;
                    }
                }

                if (j + 1 == temp.size()){
                    if (temp[j] != ','){
                        return false;
                    }else{
                        continue;
                    }
                }
                correctVal += temp[j];
            }
        }

        valuesToCol.replace(i, correctVal);
    }
    return true;
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

bool fileAvail (string nextCsv, string tablePath){ // Проверка существования CSV
    ifstream nextFileCsv(tablePath + "/" + nextCsv);
    if (nextFileCsv.is_open()) {
        return true;
    }else{ // если не существует - создаём
        ifstream firstCsv (tablePath + "/1.csv");
        string lineFirstCsv;
        getline(firstCsv, lineFirstCsv);
        firstCsv.close();

        ofstream csv(tablePath + "/" + nextCsv);
        csv << lineFirstCsv;

        return false;
    }
}

void checkCsv (string tablePath, string tableName, string& csv){
    ifstream jsonFile("files/schema.json");
    json configs;
    jsonFile >> configs;

    int turplLim = configs["tuples_limit"];
    int countCsv = 1;
    while (true){
        ifstream fileCsv(tablePath + "/" + to_string(countCsv) + ".csv");

        int lineCount = -1;
        string line;
        while (getline(fileCsv, line)){
            lineCount++;
        }
        fileCsv.close();

        if (lineCount < turplLim){
            csv = to_string(countCsv) + ".csv";
            return;
        }

        if (fileAvail(to_string(countCsv + 1) + ".csv", tablePath)){
            countCsv++;
        }else{
            countCsv++;
            csv = to_string(countCsv) + ".csv";
            return;
        }
    }
}









/*
bool correctToken (string& tempToken, string basename, string tableName){
    string table = "", column = "", val = "", temp;
    int countAp = 0;
    for (size_t i = 0; i < tempToken.size(); i++){
        if (tempToken[i] == '.'){
            table = temp;
            temp = "";
            continue;
        }
        if (tempToken[i] == '='){
            column = temp;
            temp = "";
            continue;
        }
        if (tempToken[i] == '\''){
            if (countAp == 0){
                countAp ++;
            }else if(countAp == 1){ 
                val = temp;
                temp = "";
            }else{
                cerr << "ERROR_29: Many apostrophes." << endl;
                return false;
            }
            continue;
        }
        temp += tempToken[i];
    }

    if (table == "" || column == "" || val == "" || temp != ""){
        cerr << "ERROR_21: Not enought arguments for WHERE." << endl;
        return false;
    }

    if (table != tableName){
        cerr << "ERROR_22: Unknown table name." << endl;
        return false;
    }

    ifstream csv(basename + "/" + tableName + "/1.csv");
    string line;
    getline(csv, line);

    stringstream ss(line);
    string item;
    int numberCol = 0;
    bool found = false;
    while (getline(ss, item, ';')){
        numberCol ++;
        if (item == column){
            found = true;
            break;
        }
    }
    if (found == false){
        cerr << "ERROR_24: Uncnown column." << endl;
        return false;
    }else{
        column = to_string(numberCol);
        tempToken = column + " " + val;
        return true;
    }
}

bool toTokens (string operations, StrArray& tokens, string basename, string tableName){
    stringstream ss(operations);
    string temp, temptoken;
    int tokenWords = 0, countTokens = 0, countOper = 0;
    int similarTok = 0, similarOp = 0;
    while (ss >> temp){
        if (temp == "OR" || temp == "AND"){
            if (similarOp == 1 || tokenWords != 0){
                cerr << "ERROR_16: Many 'OR' of 'AND'." << endl;
                return false;
            }
            tokens.push(temp);
            similarTok = 0;
            countOper++;
            similarOp++;
            continue;
        }

        temptoken += temp;
        tokenWords++;
        if (tokenWords == 3){
            if (similarTok == 1){
                cerr << "ERROR_19: Unknown command" << endl;
                return false;
            }
            if (!correctToken (temptoken, basename, tableName)){
                return false;
            }
            tokens.push(temptoken);
            similarOp = 0;
            similarTok++;
            countTokens++;
            temptoken = "";
            tokenWords = 0;
        }
    }

    if (tokenWords != 0){
        cerr << "ERROR_17: Unknown command." << endl;
        return false;
    }
    if (countTokens - 1 != countOper){
        cerr << "ERROR_18: Unknown command." << endl;
        return false;
    }

    return true;
}

bool findValue (string value, int numCol, string line){
    stringstream elems(line);
    string elem;
    bool isFind = false;
    int pos = 0;
    while (getline(elems, elem, ';')){
        pos++;
        if (pos == numCol && elem == value){
            isFind = true;
            break;
        }
    }
    return isFind;
}

bool isDel(StrArray& tokens, string line){
    int countOR = 0;
    bool res = false;
    for (size_t i = 0; i < tokens.sizeM(); i++){
        string token, value;
        int numCol;
        tokens.get(i, token);
        if (token == "OR"){
            if (res = true){
                return true;
            }else{
                countOR++;
                continue;
            }
        }
        stringstream elems(token);
        elems >> numCol;
        elems >> value;

        if (findValue(value, numCol, line)){
            res = true;
        }
    }

    return res;
}

*/
