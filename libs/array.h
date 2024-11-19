#ifndef ARRAY
#define ARRAY

#include <iostream>
#include <string>

using namespace std;

struct Array {
    string* data;
    size_t size, capacity;

    Array() : data(nullptr), size(0), capacity(2){
        data = new string[capacity];
    }

    void add(const string& value){
        if (size == capacity){
            capacity *= 2;
            string* newData = new string[capacity];
            for (size_t i = 0; i < size; i++){
                newData[i] = data[i];
            }
            delete[] data;
            data = newData;
        }

        data[size++] = value;
    }

    ~Array(){
        delete[] data;
    }
};

#endif // ARRAY