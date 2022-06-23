#include <vector>
#include <string>

#include <iostream>

#define TABLE_INCLUDED 1

using namespace std;

template <typename T>
class Table{
    private: 
        vector<string> index; // nombre de cada parametro e.g. i,j,k...
        vector<T> values; // valores e.g. cijkz...
        vector<int> max_params; // maximo valor de cada parametro e.g. MAXi, MAXj, MAXk ...
        vector<int> multipliers; // multiplicadores para cada indice
    public:
        Table();

        T operator[](const vector<int> &index);

        void push_back(const T value);
        void add_param(const string name, const vector<int> &set);
        void add_param(const string name, const int min, const int max);
        
};

template <typename T> Table<T>::Table(){

}

template <typename T> T Table<T>::operator[](const vector<int> &index){
    int pos = index[0], size = index.size();
    for(int i = 1; i < size; i++){
        pos += (index[i]-1)*multipliers[i-1];
    }
    return values[pos-1];
}

template <typename T> void Table<T>::push_back(const T value){
    values.push_back(value);
}

template <typename T> void Table<T>::add_param(const string name, const int min, const int max){
    int n_mult = multipliers.size();

    index.push_back(name);
    max_params.push_back(max);
    if(n_mult > 0){
        multipliers.push_back(max*multipliers[n_mult-1]);
    }
    else{
        multipliers.push_back(max);
    }
}


