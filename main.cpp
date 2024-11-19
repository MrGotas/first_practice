#include <iostream>
#include <string>
#include <sstream>
#include "libs/array.h"
#include "libs/read_json.h"

using namespace std;
namespace fs = filesystem;

void insCom(string userCommand);

int main() {
    /*
    cout << "Напиши чего ты хочешь: ";
    string userWish;
    cin >> userWish;
    cout << "Если хочется  " << userWish << " - перехочется." << endl; 
    cout << "Сиди учись, пока ещё студент" << endl;
    */

    Array massTabl;
    string baseName;
    readJson(&massTabl, baseName);

    string userCommand;
    while (true){
        getline(cin, userCommand);

        stringstream ss(userCommand);
        string temp;
        ss >> temp;

        if (temp == "EXIT"){
            return 0;
        }else if (temp == "INSERT"){
            insCom(userCommand);
        }else if (temp == "DELETE"){

        }else if (temp == "SELECT"){

        }else{
            cout << "ERROR_1: Unknown command." << endl;
        }
    }

    return 0;
}

void insCom(string userCommand) {
    stringstream ss(userCommand);
    string word;
    int countWor = 0;

    while (ss >> word){
        countWor ++;
        cout << word << endl;

        if (countWor == 2 && word != "INTO"){
            cout << "ERROR_2: Unknown command." << endl;
            return;
        }
        if (countWor == 3){

        }
    }
    if (countWor != 6){
        cout << "ERROR_3: Unknown command." << endl;
        return;
    } 
}