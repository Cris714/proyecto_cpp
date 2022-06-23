#include <iostream>
#include <fstream>
#include <sstream>
#include <map>
#include <unordered_map>
#include <vector>

#ifndef TABLE_INCLUDED
#include "table.h"
#endif

#define MAX_WIDTH 70

using namespace std;


template <typename T> void load_file_to_map(const string &path, map<vector<int>, T> &values){
    vector<int> key;
    string line, word;

    fstream file(path, ios::in);
    if(file.is_open()){
        getline(file, line);
        while(getline(file, line)){
            stringstream str(line);
            key.clear();
            while(getline(str, word, ',')){
                key.push_back(stoi(word));
            }
            key.pop_back();
            values[key] = stod(word);
        }
    }
}

template <typename T> void load_file_to_hashmap(const string &path, unordered_map<vector<int>, T> &values){
    vector<int> key;
    string line, word;

    fstream file(path, ios::in);
    if(file.is_open()){
        getline(file, line);
        while(getline(file, line)){
            stringstream str(line);
            key.clear();
            while(getline(str, word, ',')){
                key.push_back(stoi(word));
            }
            key.pop_back();
            values[key] = stod(word);
        }
    }
}

template <typename T> void load_file_to_table(const string &path, Table<T> &table){
    string line, word;
    fstream file(path, ios::in);
    if(file.is_open()){
        getline(file, line);
        while(getline(file, line)){
            stringstream str(line);
            while(getline(str, word, ','));
            table.push_back(stod(word));
        }
    }
}

void create_or_empty(string path){
    ofstream file(path);
    file.close();
}

void append_to_file(const string& path, const string &text){
    ofstream file(path, ios::app);
    string line = "";
    const int text_length = text.length();

    line += text[0];
    for(int i = 1; i < text_length; i++){
        if(i%MAX_WIDTH == 0){
            line += "\n";
        }
        line += text[i];
    }
    file << line;
    file.close();
}

