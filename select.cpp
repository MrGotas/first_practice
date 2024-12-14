#include "libs/select.h"

void selectCom (string userCommand, string baseName){
    stringstream ss(userCommand);
    string temp;
    ss >> temp; // SELECT
    
    StrArray outputCol;
    bool wasFrom = false;
    while (ss >> temp){ // значения после SELECT
        if (temp == "FROM"){
            wasFrom = true;
            break;
        }

        if (!checkOutputCol(temp, baseName)){
            cerr << "ERROR_13: Unknown condition." << endl;
            return;
        }
        outputCol.push(temp);
    }

    if (!wasFrom){ // не встретился FROM
        cerr << "ERROR_14: Unknown condition." << endl;
        return;
    }

    StrArray fromTable;
    bool wasWhere = false;
    while (ss >> temp){ // значения после FROM
        if (temp == "WHERE"){
            wasWhere = true;
            break;
        }
        removeComma(temp);
        fromTable.push(temp);
    }

    if (!checkFrom(fromTable, outputCol)){
        cerr << "ERROR_15: Unknown condition." << endl;
        return;
    }

    //outputCol.print();
    //fromTable.print();

    if (isLockTables(fromTable, baseName)){ //Проверка блокировки таблицы
        cout << "The table is currently locked for use, try again later." << endl;
        return;
    }else{
        lockTables(fromTable, baseName, "1"); // блокируем на время работы
    }

    createTemp(outputCol, baseName, wasWhere);

    massWithoutCol(outputCol);

    int countColumns = outputCol.sizeM() - 1, nowTable = 0;
    string output, colNames, tempStr;
    crossJoin(outputCol, baseName, countColumns, nowTable, output, colNames, tempStr);
    cout << colNames << endl;
    cout << output << endl;



    lockTables(fromTable, baseName, "0");
}

void lockTables(StrArray& tables, string baseName, string command){
    for (size_t i = 0; i < tables.sizeM(); i++){
        string tableName = tables.get(i);
        string tablePath = baseName + "/" + tableName;
        string path = tablePath + "/" + tableName + "_lock";
        ofstream file(path);
        file << command;
        file.close();
    }
}

bool isLockTables(StrArray& tables, string baseName){
    for (size_t i = 0; i < tables.sizeM(); i++){
        string tableName = tables.get(i);
        string tablePath = baseName + "/" + tableName;
        string path = tablePath + "/" + tableName + "_lock";

        ifstream file (path);

        int value;
        file >> value;
        file.close();

        if(value == 1){
            return true;
        }
    }
    return false;
}

string getValueFromLine(string line, int columnNum){
    string value;
    int checkedPos = 0;
    stringstream values(line);
    while (getline(values, value, ';')){
        if (checkedPos == 0){
            checkedPos++;
            continue;
        }
        checkedPos++;
        if (checkedPos == columnNum){
            break;
        }
    }
    return value;
}

void fromCsvToTemp (string baseName, string tableName, string column){ // функция чтения всех csv одной таблицы
    string tempPath = baseName + "/" + tableName + "/temp.csv";
    int columnNum = colIndex(tableName, column);
    int countCsv = 1;
    string columnName = " ";
    while (true){
        string csvPath = baseName + "/" + tableName + "/" + to_string(countCsv) + ".csv";
        if (!csvAvail(csvPath)){
            break;
        }
        
        ifstream csv(csvPath);
        string line;
        if (columnName == " "){ // записываем название столбца отдельно
            getline(csv, line);
            string value = getValueFromLine(line, columnNum);
            ofstream tempFile(tempPath, ios::app);
            tempFile << value;
            columnName = value;
            tempFile.close();
        }else{
            getline(csv, line);
        }

        ofstream tempFile(tempPath, ios::app);
        while (getline(csv, line)){
            string value = getValueFromLine(line, columnNum); // получаем значение со строки

            tempFile << endl << value;
        }
        
        tempFile.close();
        countCsv++;
    }
}

void createTemp(StrArray& outputCol, string baseName, bool wasWhere){
    if (wasWhere == false){
        for (size_t i = 0; i < outputCol.sizeM(); i++){
            string tableCol = outputCol.get(i);

            string tableName, column;
            getTablCol(tableCol, tableName, column); // оплучаем отдельно таблицу и колонку
            
            fromCsvToTemp(baseName, tableName, column);
        }

    }else{

    }
}

void massWithoutCol (StrArray& outputCol){
    for (size_t i = 0; i < outputCol.sizeM(); i++){
        string elem = outputCol.get(i);
        stringstream values(elem);
        string tableName;
        getline(values, tableName, '.');
        outputCol.replace(i, tableName);
    }
}

void crossJoin(StrArray& outputCol, string baseName, int countColumns, int nowTable, string& output, string& colNames, string& tempStr){
    string tableName;
    for (size_t i = 0; i < outputCol.sizeM(); i++){
        if (i == nowTable){
            tableName = outputCol.get(i);
            break;
        }
    }
    
    //cout << "Сейчас в таблице " << tableName << endl;

    string tempPath =  baseName + "/" + tableName + "/temp.csv";
    ifstream tempFile(tempPath);
    string value;
    getline(tempFile, value);
    while (getline(tempFile, value)){

        if (nowTable == countColumns){
            output += tempStr + value + "\n";
        }else{
            tempStr += value + " ";
            //cout << "На данный момент значения " << output << endl;
            if (nowTable < countColumns){
                crossJoin(outputCol, baseName, countColumns, nowTable + 1, output, colNames, tempStr);
            }
        }
    }
    tempStr = "";
}

//SELECT engineer.modelWork, passengers.modelAirplane, airplane.model FROM engineer, passengers, airplane

bool checkFrom (StrArray& fromTable, StrArray& outputCol){
    for (size_t i = 0; i < fromTable.sizeM(); i++){
        string table = fromTable.get(i);
        for (size_t i = 0; i < outputCol.sizeM(); i++){
            string elem = outputCol.get(i);
            stringstream ss(elem);
            getline(ss,elem,'.');
            if (elem == table){
                break;
            }else if (i == outputCol.sizeM() - 1){ // если так и не нашел значений
                return false;
            }
        }
    }
    return true;
}

bool checkOutputCol(string& tableCol, string baseName){
    removeComma(tableCol); // удаляем запятую, если она есть в конце

    string table, column;
    getTablCol(tableCol, table, column);

    string tablePath = baseName + "/" + table;
    if (!checkTable(tablePath)){
        return false;
    }

    if(!columnExist(table, column)){
        return false;
    }

    return true;
}

void removeComma(string& value){
    if (value[value.size() - 1] == ','){
        string result;
        for (size_t i = 0; i < value.size() - 1; i++){
            result += value[i];
        }
        value = result;
    }
}