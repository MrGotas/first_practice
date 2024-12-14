#include "libs/delete.h"

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

    if (!correctCond(condArr, tableName)){
        cerr << "ERROR_12: Unknown condition." << endl;
        return;
    }

    if (isLock(tablePath, tableName)){ //Проверка блокировки таблицы
        cout << "The table is currently locked for use, try again later." << endl;
        return;
    }else{
        lockTable(tablePath, tableName); // блокируем на время работы
    }

    StrArray tokens;
    toTokens(condArr, tokens);

    int turplLim = getTurplLim();
    int countCsv = 1, countTemp = 1;
    int countAddStr = 0; // кол-во добавленных строк в temp файле
    bool stop = false;
    while (stop != true){
        if (stop == true){
            break;
        }
        string csv = tablePath + "/" + to_string(countCsv) + ".csv";
        if (!csvAvail(csv)){
            break;
        }
        ifstream fileCsv(csv);
        string line, firstLine;
        int lineCount = -1;
        while (getline(fileCsv,line)){
            if (lineCount == -1){
                firstLine = line;
                if(fileCsv.eof()){ // если csv пустой
                    ofstream file (tablePath + "/1temp.csv");
                    file << firstLine;
                    file.close();
                    stop = true;
                }
                lineCount++;
                continue;
            }

            if(!checkCond(tokens, line)){ // запись в новый temp файл, т.к. старый заполнился
                if (countAddStr == turplLim){
                    countAddStr = 0;
                    countTemp++;
                }
                string tempFile = tablePath + "/" + to_string(countTemp) + "temp.csv";
                addToTemp(line, tempFile, firstLine);
                countAddStr ++;
            }
            
            lineCount++;
            if (lineCount == turplLim){
                countCsv++;
            }else if(fileCsv.eof()){ // на последней строке файла
                stop = true;
            }
        }
        fileCsv.close();
        if (countAddStr == 0){
            ofstream file (tablePath + "/1temp.csv");
            file << firstLine;
            file.close();
        }
    }

    int delCsv = 1;
    while (delCsv <= countCsv){ // удаление старых csv
        fs::remove(tablePath + "/" + to_string(delCsv) + ".csv");
        delCsv++;
    }

    int renameTemp = 1;
    while (renameTemp <= countTemp){ // переименовывание temp файлов
        fs::rename(tablePath + "/" + to_string(renameTemp) + "temp.csv", tablePath + "/" + to_string(renameTemp) + ".csv");
        renameTemp++;
    }

    unlockTable(tablePath, tableName);
}

bool correctCond(StrArray& condArr, string tableName){
    int numElem = 0, countTokens = 1, countLogicOp = 0; // елементов просмотренно
    for (size_t i = 0; i < condArr.sizeM(); i++){
        string elem = condArr.get(i);
        string temp; // записываем готовый токен
        if (numElem > 3){
            return false;
        }else if (elem == "="){
            if (numElem != 1){
                return false;
            }else{
                numElem++;
            }
        }else if (numElem == 0){
            stringstream ss (elem);
            string inputTabNam;
            getline(ss, inputTabNam, '.');
            if (inputTabNam != tableName){
                return false;
            }

            string column;
            getline(ss, column);
            if (!columnExist(tableName, column)){
                return false;
            }
            numElem++;
        }else if(numElem == 2){
            if (elem.size() < 3 || elem[0] != '\'' || elem[elem.size() - 1] != '\''){
                return false;
            }else{
                numElem++;
            }
        }else if (elem == "OR" || elem == "AND"){
            if (numElem != 3){
                return false;
            }else{
                numElem = 0;
                countTokens++;
                countLogicOp++;
            }
        }else{
            return false;
        }
    }

    if (countLogicOp != countTokens - 1){ // OR или AND > либо < токенов
        return false;
    }else if (numElem != 3){ // недозаполненные значения
        return false;
    }else{
        return true;
    }
}

void toTokens(StrArray& condArr, StrArray& tokens){
    int countLogicOp = condArr.sizeM() / 4; // количество OR/AND в массиве
    if (countLogicOp == 0){
        string token = condArr.get(0) + " " + condArr.get(2);
        tokens.push(token);
        return;
    }
    int viewLogOp = 0; // сколько из них просмотренно
    bool wasOr = false, wasAnd = false; // маркеры просмотренных операторов
    for (size_t i = 3; i < condArr.sizeM(); i += 4){
        string elem = condArr.get(i);
        string token = condArr.get(i - 3) + " " + condArr.get(i - 1);
        string nextToken = condArr.get(i + 1) + " " + condArr.get(i + 3);
        if (elem == "OR"){
            viewLogOp++;
            if (wasAnd == false){
                tokens.push(token);
            }else{
                replaceTok (tokens, token);
            }

            if (viewLogOp == countLogicOp){
                tokens.push(nextToken);
            }
            wasOr = true;
            wasAnd = false;
        }else if (elem == "AND"){
            viewLogOp++;
            if (wasOr == true){
                tokens.push(token);
            }else{
                if (tokens.sizeM() == 0){
                    tokens.push(token);
                }else{
                    replaceTok (tokens, token);
                }
            }

            if (viewLogOp == countLogicOp){
                replaceTok (tokens, nextToken);
            }
            wasOr = false;
            wasAnd = true;
        }
    }
}

void replaceTok(StrArray& tokens, string nextToken){
    string lastTok = tokens.get(tokens.sizeM() - 1);
    tokens.replace(tokens.sizeM() - 1, lastTok + " " + nextToken);
}

string withoutApostr (string word){
    string result;
    for (size_t i = 1; i < word.size() - 1; i++){
        result += word[i];
    }
    return result;
}

bool checkVals (string line, string condVal, int valuePos){
    int checkedPos = 0;
    stringstream ss (line);
    string value;
    while(getline(ss, value, ';')){
        if (checkedPos == 0){
            checkedPos++;
            continue;
        }
        checkedPos++;
        if (checkedPos == valuePos){
            if(value == condVal){
                return true;
            }
        }
    }
    return false;
}

bool checkRes (string result){
    stringstream ss (result);
    string temp;
    while (ss >> temp){
        if (temp == "false"){
            return false;
        }
    }
    return true;
}

void addToTemp (string line, string tempPath, string firstLine){
    if (!fs::exists(tempPath)){
        ofstream file (tempPath);
        file << firstLine << endl << line;
        file.close();
        return;
    }
    ofstream file (tempPath, ios::app);
    file << endl << line;
    file.close();
}

bool checkCond (StrArray& tokens, string line){
    for (size_t i = 0; i < tokens.sizeM(); i++){ // проходимся по токенам
        string token = tokens.get(i);
        stringstream ss(token);
        int countArg = 0, valuePos;
        string temp, table, column, condVal;
        string result = "";
        while (ss >> temp){
            if (countArg == 0){
                countArg ++;
                getTablCol(temp, table, column); // отдельно название таблицы и стобец
                valuePos = colIndex(table, column); // позиция значения в строке csv
            }else{
                countArg = 0;
                condVal = withoutApostr(temp);
                if (checkVals (line, condVal, valuePos)){
                    result += " true";
                }else{
                    result += " false";
                }
            }
        }
        
        if (checkRes(result)){
            return true;
        }
    }
    return false;
}