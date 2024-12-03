#include "libs/insert.h"

void insertCom(string userCommand, string baseName){
    string tableName, values;
    if (!checkSyntax(tableName, values, baseName, userCommand, "INS")){
        return;
    }

    string tablePath = baseName + "/" + tableName;
    if (!checkTable(tablePath)){
        cerr << "ERROR_5: Unknown table name." << endl;
        return;
    }

    stringstream ss (values);
    string temp;
    StrArray valuesToCol;
    while (ss >> temp){
        valuesToCol.push(temp);
    }

    int countCols = countCol(tableName); // кол-во столбцов в таблице

    if (countCols != valuesToCol.sizeM()){
        cerr << "ERROR_6: Incorrect count of values." << endl;
        return;
    }

    if (checkValues(valuesToCol) == false){ //проверка синтаксиса вводимых значений
        cerr << "ERROR_7: Incorrect syntax values." << endl;
        return;
    }

    if (isLock(tablePath, tableName)){ //Проверка блокировки таблицы
        cout << "The table is currently locked for use, try again later." << endl;
        return;
    }else{
        lockTable(tablePath, tableName); // блокируем на время работы
    }

    string pkFile = tablePath + "/" + tableName + "_pk";
    ifstream filePk(pkFile);
    if (!filePk.is_open()){
        cerr << "ERROR_10: Unable to open file: " << pkFile << endl;
        unlockTable(tablePath, tableName);
        return;
    }
    int pkVal;
    filePk >> pkVal;
    filePk.close();

    string dataToTable; // строка к вставке
    dataToTable = to_string(pkVal) + ";";
    for (size_t i = 0; i < valuesToCol.sizeM(); i++){
        string elem;
        valuesToCol.get(i, elem);
        dataToTable += elem + ";";
    }

    string csv; // выбор csv файла в который будем записывать строку
    checkCsv (tablePath, tableName, csv);

    ofstream fileCsv(tablePath + "/" + csv, ios::app);
    fileCsv << endl << dataToTable;
    fileCsv.close();

    ofstream filePkEnd(pkFile);
    filePkEnd << pkVal + 1;

    unlockTable(tablePath, tableName);
}