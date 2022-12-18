#include <iostream>
#include <vector>
#include "objective_and_constraints.h"

using namespace std;

int main(){
    unsigned short m = 11;

    for(unsigned short i: C(m)){
        cout << i << " ";
    }
    cout << endl;

    return EXIT_SUCCESS;
}