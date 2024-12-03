#include <iostream>
#include <string>
#include <sstream>
#include "libs/read_json.h"
#include "libs/insert.h"
#include "libs/delete.h"

using namespace std;

int main() {
    /*
    cout << "Напиши чего ты хочешь: ";
    string userWish;
    cin >> userWish;
    cout << "Если хочется  " << userWish << " - перехочется." << endl; 
    cout << "Сиди учись, пока ещё студент" << endl;
    */
    string baseName;
    readJson(baseName);

    string userCommand;
    while (true){
        getline(cin, userCommand);

        stringstream ss(userCommand);
        string temp;
        ss >> temp;

        if (temp == "EXIT"){
            return 0;
        }else if (temp == "INSERT"){
            insertCom(userCommand, baseName);
        }else if (temp == "DELETE"){
            delCom(userCommand, baseName);
        }else if (temp == "SELECT"){

        }else{
            cerr << "ERROR_1: Unknown command." << endl;
        }
    }

    return 0;
}