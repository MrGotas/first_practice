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

    string colNames;
    createTemp(outputCol, baseName, wasWhere, colNames);

    massWithoutCol(outputCol);

    int nowTable = 0;
    string output, tempStr;
    crossJoin(baseName, outputCol, nowTable, output, tempStr);
    cout << colNames << endl;
    //cout << output;



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

void fromCsvToTemp (string baseName, string tableName, string column, string& colNames){ // функция чтения всех csv одной таблицы
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
            colNames += value + " ";

            getline(csv, line); // записываем первое значение после названия колонки
            ofstream tempFile(tempPath, ios::app);
            value = getValueFromLine(line, columnNum);
            tempFile << value;
            columnName = value;
        }else{
            getline(csv, line);
        }

        ofstream tempFile(tempPath, ios::app);
        while (getline(csv, line)){
            string value = getValueFromLine(line, columnNum); // получаем значение со строки

            tempFile << endl << value;
        }
        
        csv.close();
        tempFile.close();
        countCsv++;
    }
}

void createTemp(StrArray& outputCol, string baseName, bool wasWhere, string& colNames){
    if (wasWhere == false){
        for (size_t i = 0; i < outputCol.sizeM(); i++){
            string tableCol = outputCol.get(i);

            string tableName, column;
            getTablCol(tableCol, tableName, column); // оплучаем отдельно таблицу и колонку
            
            fromCsvToTemp(baseName, tableName, column, colNames);
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

void clearTemp(string& tempStr){
    cout << "------------------------------------------------------------------------" << endl;
    stringstream counter(tempStr);
    string element;
    int countWords = 0;
    while (counter >> element) {
        countWords++;
    }

    if (countWords == 1) {
        tempStr = "";
        return;
    }

    stringstream words(tempStr);
    string word, result;
    int count = 0;
    while (words >> word) {
        if (count == countWords - 1) {
            break;
        } else {
            result += word + " ";
        }
        count++;
    }
    cout << "Temp = " << tempStr << " result " << result << endl;
    cout << "---------------------------------------------------------------------------" << endl;
    tempStr = result;
}

void crossJoin(string baseName, StrArray& outputCol, int nowTable, string& output, string tempStr){
    string tableName;
    for (size_t i = 0; i < outputCol.sizeM(); i++){
        if (i == nowTable){
            tableName = outputCol.get(i);
            nowTable++;
            break;
        }
    }
    cout << "Таблица " << tableName << " номер " << nowTable << " всего " << outputCol.sizeM() << endl;
    
    string tempPath = baseName + "/" + tableName + "/temp.csv";
    ifstream tempFile(tempPath);
    string value;

    while (getline(tempFile, value)){
        if (nowTable != outputCol.sizeM()){ // если рассматривается не последняя таблица
            tempStr += value + " ";

            cout << "Сейчас в таблице " << tableName << " " << endl;
            cout << "tempStr = " << tempStr << endl;
            cout << "Перехожу в другую таблицу " << endl;

            crossJoin(baseName, outputCol, nowTable, output, tempStr);
            clearTemp(tempStr); // убераем послденее добавленное значение

        }else{// в случае рассмотрения последней таблицы
            cout << "В последней таблице " << endl;
            string result = tempStr + " " + value;
            cout << "Записываю в output: " << result << endl;
        }
    }

    /*
    while (getline(tempFile, value)){
        cout << tableName << " " << nowTable << endl;
        if (nowTable != outputCol.sizeM() - 1){ // если рассматривается не последняя таблица
        
            crossJoin(baseName, outputCol, nowTable, output, tempStr);

        }else{ // в случае рассмотрения последней таблицы
            output += tempStr + value + "\n";
        }
    }*/
}

//SELECT engineer.modelWork, passengers.modelAirplane, airplane.model FROM engineer, passengers, airplane
//SELECT engineer.modelWork, passengers.modelAirplane FROM engineer, passengers

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