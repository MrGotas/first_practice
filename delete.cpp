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
}

bool correctCond(StrArray& condArr, string tableName){
    int numElem = 0, countTokens = 1, countLogicOp = 0; // елементов просмотренно
    for (size_t i = 0; i < condArr.sizeM(); i++){
        string elem = condArr.get(i);
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