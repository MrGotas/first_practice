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
    int countWords;

    while (ss >> word){
        countWords++;
    }

    return countWords;
}

bool checkTable (string tablePath, int& countCol){
    if (fs::exists(tablePath) && fs::is_directory(tablePath)){

        string pathCSV = tablePath + "/1.csv";
        ifstream file(pathCSV);

        string columns;
        getline(file, columns);
        stringstream ss(columns);
        string column;

        while (getline (ss, column, ';')){
            countCol++;
        }
        countCol--;
        return true;
    }else{
        return false;
    }
}

bool isLock(string tablePath, string tableName){
    string path = tablePath + "/" + tableName + "_lock";
    ifstream file (path);
    if (!file.is_open()){
        cerr << "ERROR_6: Unable to open file: " << path << endl;
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
        cerr << "ERROR_7: Unknown value from file: " << path << endl;
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

bool fileAvail (string nextCsv, string tablePath){ // Проверка существования Csv
    ifstream nextFileCsv(tablePath + "/" + nextCsv);
    if (nextFileCsv.is_open()) {
        return true;
    }else{
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



