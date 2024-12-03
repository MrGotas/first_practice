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
        return;
    }
}

bool correctCond(StrArray& condArr, string tableName){
    int numElem = 0;
    for (size_t i = 0; i < condArr.sizeM(); i++){
        string elem = condArr.get(i); 
        if (numElem == 0){
            
        }
    }
}