#include <iostream>
#include <vector>
#include <map>
#include <chrono>

#include "file_manager.h"
#include "funcs.h" 


using namespace std;



int main(){
    const string out_filename = "../out/output.lp";
    const string out_columns = "../out/columns.csv";
    const string out_rows = "../out/rows.csv";


    // VARIABLES 

    string str_out; // string con la funcion/restriccion construida
    string str_col; // string con información: nombre_restriccion | variable | coeficiente
    string str_row; // string con información: variable/retriccion | signo | valor_RHI


    // GENERACION DE ESTRUCTURAS CON DATOS DE ARCHIVOS

    map<vector<unsigned short>, float> cambio;
    map<vector<unsigned short>, float> factor;
    map<vector<unsigned short>, float> fores;
    map<vector<unsigned short>, float> raleo;
    map<vector<unsigned short>, float> reconversion; 
    map<vector<unsigned short>, float> rendimientos; 
    map<vector<unsigned short>, float> superficie; 

    load_file_to_map<unsigned short, float>("../data/cambio.csv", cambio);
    load_file_to_map<unsigned short, float>("../data/factor.csv", factor);
    load_file_to_map<unsigned short, float>("../data/fores.csv", fores);
    load_file_to_map<unsigned short, float>("../data/raleo.csv", raleo);
    load_file_to_map<unsigned short, float>("../data/rendimientos.csv", rendimientos);
    load_file_to_map<unsigned short, float>("../data/superficie.csv", superficie);
    load_file_to_map<unsigned short, float>("../data/reconversion.csv", reconversion);


    // CREACION DEL ARCHIVO DE SALIDA

    create_or_empty(out_filename);
    append_to_file(out_filename, "Max\n");

    create_or_empty(out_columns);
    append_to_file(out_columns, "restriccion,variable,coeficiente\n");

    create_or_empty(out_rows);
    append_to_file(out_rows, "restriccion,signo,valor RHI\n");


    // FUNCION OBJETIVO 
    
    objective_function(str_out, str_col, str_row, rendimientos, factor);
    append_to_file(out_filename, str_out+" ", 'f');
    append_to_file(out_columns, str_col);
    append_to_file(out_rows, str_row);
    str_out = str_col = str_row = "";
    cout << "Funcion objetivo: LISTO" << endl;

    
    // RESTRICCIONES
    
    append_to_file(out_filename, "\nSubject to");
    

    // (AI) Restricción de inventario inicial 
    constraint1(str_out, str_col, str_row, superficie);
    append_to_file(out_filename, str_out, 'r');
    append_to_file(out_columns, str_col);
    append_to_file(out_rows, str_row);
    str_out = str_col = str_row = "";
    cout << "Restriccion 1: LISTO" << endl;

    
    // (SC) Cuantificación de superficie cosechada
    constraint2(str_out, str_col, str_row);
    append_to_file(out_filename, str_out, 'r');
    append_to_file(out_columns, str_col);
    append_to_file(out_rows, str_row);
    str_out = str_col = str_row = "";
    cout << "Restriccion 2: LISTO" << endl;

    
    // (VC) Cuantificación de volumen  de corta final
    constraint3(str_out, str_col, str_row, rendimientos, factor);
    append_to_file(out_filename, str_out, 'r');
    append_to_file(out_columns, str_col);
    append_to_file(out_rows, str_row);
    str_out = str_col = str_row = "";
    cout << "Restriccion 3: LISTO" << endl;

    
    // (RET) Límite de superficie de reconversión
    constraint4(str_out, str_col, str_row, reconversion);
    append_to_file(out_filename, str_out, 'a');
    append_to_file(out_columns, str_col);
    append_to_file(out_rows, str_row);
    str_out = str_col = str_row = "";
    cout << "Restriccion 4: LISTO" << endl;


    // (LST) Exclusión superficie de reconversión
    constraint5(str_out, str_col, str_row);
    append_to_file(out_filename, str_out, 'a');
    append_to_file(out_columns, str_col);
    append_to_file(out_rows, str_row);
    str_out = str_col = str_row = "";
    cout << "Restriccion 5: LISTO" << endl;


    // (TT) Límite de superficie con cambio de especie
    constraint6(str_out, str_col, str_row, cambio);
    append_to_file(out_filename, str_out, 'a');
    append_to_file(out_columns, str_col);
    append_to_file(out_rows, str_row);
    str_out = str_col = str_row = "";
    cout << "Restriccion 6: LISTO" << endl;


    // (RFA) Reforestación A (superficie disponible para reforestar con o sin cambio de especie)
    constraint7(str_out, str_col, str_row);
    append_to_file(out_filename, str_out, 'a');
    append_to_file(out_columns, str_col);
    append_to_file(out_rows, str_row);
    str_out = str_col = str_row = "";
    cout << "Restriccion 7: LISTO" << endl;


    // (RFB) Reforestación B (superficie a reforestar con una especie igual o distinta a la anterior)
    constraint8(str_out, str_col, str_row, cambio);
    append_to_file(out_filename, str_out, 'a');
    append_to_file(out_columns, str_col);
    append_to_file(out_rows, str_row);
    str_out = str_col = str_row = "";
    cout << "Restriccion 8: LISTO" << endl;


    // (RT) Cuantificacion de volumen de raleos
    constraint9(str_out, str_col, str_row, raleo);
    append_to_file(out_filename, str_out, 'a');
    append_to_file(out_columns, str_col);
    append_to_file(out_rows, str_row);
    str_out = str_col = str_row = "";
    cout << "Restriccion 9: LISTO" << endl;


    // (FT) Superficie disponible para forestación
    constraint10(str_out, str_col, str_row, fores);
    append_to_file(out_filename, str_out, 'a');
    append_to_file(out_columns, str_col);
    append_to_file(out_rows, str_row);
    str_out = str_col = str_row = "";
    cout << "Restriccion 10: LISTO" << endl;



    append_to_file(out_filename, "\nend");
    

    cout << "***FIN DEL PROGRAMA. SALIDA GENERADA CON EXITO***" << endl;


    return 0;
}



