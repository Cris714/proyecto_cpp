#include <iostream>
#include <vector>
#include <map>
#include <chrono>

#include "file_manager.h"
#include "objective_and_constraints.h" 


using namespace std;


// DEFINICION DE CONJUNTOS
#ifndef _SETS
#define P {1,2,3,4} // propietario
#define R {5,6,7,8,9,10,13,14} // region administrativa
#define Z {1,2,4,5,6,7,9} // zona de crecimiento
#define S {1,2,3,4} // clases de sitio
#define M {1,2,3,7,8,9,10,11,12} // esquemas de manejo
#define K {1,2,3,4,5,6,7,8,9,10,11,12,13} // producto
#define I_MAX 30 // año de plantacion (-)
#endif



int main(){
    const string out_filename = "../out/output.lp";


    // VARIABLES 

    string str_out; // string con la funcion/restriccion construida


    // GENERACION DE MAPA DE RENDIMIENTO Y SUPERFICIE

    map<vector<unsigned short>, float> perf; // rendimiento
    map<vector<unsigned short>, float> surf; // superficie

    load_file_to_map<unsigned short, float>("../data/rendimientos.csv", perf);
    load_file_to_map<unsigned short, float>("../data/superficie.csv", surf);


    // CREACION DEL ARCHIVO DE SALIDA

    create_or_empty(out_filename);
    append_to_file(out_filename, "Max\n");


    // FUNCION OBJETIVO 

    objective_function(str_out, perf);
    append_to_file(out_filename, str_out, 'f');
    str_out = "";
    cout << "Funcion objetivo: LISTO" << endl;


    // RESTRICCIONES

    append_to_file(out_filename, "\nSubject to");

    // Restricción de inventario inicial
    constraint1(str_out, surf);
    append_to_file(out_filename, str_out, 'r');
    str_out = "";
    cout << "Restriccion 1: LISTO" << endl;

    // // Cuantificación de superficie cosechada
    // constraint2(str_out, surf);
    // append_to_file(out_filename, str_out, 'r');
    // str_out = "";
    // cout << "Restriccion 2: LISTO" << endl;

    // // Cuantificación de volumen  de corta final
    // constraint3(str_out, perf);
    // append_to_file(out_filename, str_out, 'r');
    // str_out = "";
    // cout << "Restriccion 3: LISTO" << endl;

    // // Exclusión superficie de reconversión 
    // constraint4(str_out, perf);
    // append_to_file(out_filename, str_out, 'r');
    // str_out = "";
    // cout << "Restriccion 4: LISTO" << endl;



    append_to_file(out_filename, "\nend");


    cout << "***FIN DEL PROGRAMA. SALIDA GENERADA CON EXITO***" << endl;


    return 0;
}



