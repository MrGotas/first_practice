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

    //DELETE FROM table1 WHERE table1.column4 = 'sdf' OR table1.column3 = 'sdfw' AND table1.column1 = 'heheh'
    StrArray tokens;
    toTokens(condArr, tokens);
    tokens.print();

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

void replaceTok(StrArray& tokens, string nextToken){
    string lastTok = tokens.get(tokens.sizeM() - 1);
    tokens.replace(tokens.sizeM() - 1, lastTok + " " + nextToken);
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






