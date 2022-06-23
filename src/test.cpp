#include <iostream>
#include <map>
#include <vector>

#include "table.h"
#include "file_manager.h"

using namespace std;

int main(){
    
    Table<int> data;
    load_file_to_table("./constantes.txt", data);

    data.add_param("i", 1, 10);
    data.add_param("j", 1, 2);

    cout << data.get({4,1}) << endl;

    return 0;
}