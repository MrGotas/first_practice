#include "libs/read_json.h"

void readJson(Array* massTabl, string& baseName){

    ifstream jsonFile("files/schema.json");
    json configs;
    jsonFile >> configs;

    string schemaName = configs["name"];

    fs::create_directory(schemaName);

    for (const auto& [tableName, columns] : configs["structure"].items()) {
        string tablePath = schemaName + "/" + tableName;
        fs::create_directory(tablePath);

        if (!fs::exists(tablePath + "/" + tableName + "_pk")){
            ofstream pkFile(tablePath + "/" + tableName + "_pk");
            pkFile << "1";
            pkFile.close();

            ofstream lockFile(tablePath + "/" + tableName + "_lock");
            lockFile << "0";
            lockFile.close();

            ofstream csvFile(tablePath + "/1.csv");
            csvFile << tableName + "_pk;";
            for (const auto& column : columns){
                csvFile << remQuotes(column) << ";";
            }
            csvFile.close();
        }

    }
}

string remQuotes(string column){
    string result;
    for (char symbol : column){
        if (symbol != '"'){
            result += symbol;
        }
    }
    return result;
}