#include <iostream>
#include <fstream>
#include <sstream>
#include <map>

#include "file_manager.h"

using namespace std;


map<string, string> global_indexes; // contiene los nombres de cada indice, se llena en main


// void convert_output_to_csv(const string &in_filename, const string &out_filename){
//     string fname, index, line, word, indexes, text = "";
//     string format = out_filename.substr(out_filename.rfind('.'), out_filename.length()); // consigue formato de salida
//     bool is_var;
//     char var_type = ' ';
//     char prev_var_type = var_type;
//     int size;

//     fstream file_in(in_filename, ios::in);

//     if(file_in.is_open()){
//         while(getline(file_in, line) && line.rfind("Variable Name", 0) != 0); // itera hasta conseguir el string de rfind
//         while(getline(file_in, line) && line[0] != 'A'){ // extrae variables hasta encontrar la 'A' de "All other variables..."
//             stringstream str(line);
//             is_var = true;
//             while(getline(str, word, ' ')){
//                 if(word[0] == '\0') continue; // caracter nulo
//                 if(is_var){
//                     var_type = word[0];
//                     indexes = word.substr(1, word.length());
//                     text += word+",";
//                     is_var = false;
//                 }
//                 else{
//                     text += word.substr(0,word.length()-1)+",";
//                 }
//             }
//             size = indexes.length();

//             if(var_type != prev_var_type && prev_var_type != ' '){
//                 fname = out_filename.substr(0, out_filename.length()-format.length())+"_"+prev_var_type+format;
//                 create_or_empty(fname);
//                 append_to_file(fname, global_indexes[prev_var_type]+"\n");
//                 append_to_file(fname, text.substr(0, text.rfind('\n')));
//                 text = text.substr(text.rfind('\n')+1, text.length());  
//             }
//             prev_var_type = var_type;

//             switch(var_type){
//                 case 'Y': // 'Y' tiene un indice especial
//                     for(int i = 0; i < size; i+=2){ // se asume size par
//                         index = indexes.substr(i,2);
//                         if(index[0] == '0') index = index[1];
//                         text += (i==size-4? "-":"")+index+(i < size-2? ",":"\n");
//                     }
//                     break;

//                 default: // 'V', 'S'
//                     for(int i = 0; i < size; i+=2){ // se asume size par
//                         index = indexes.substr(i,2);
//                         if(index[0] == '0') index = index[1];
//                         text += index+(i < size-2? "," : "\n");
//                     }
//                     break;
//             }
//         }
//     }

//     fname = out_filename.substr(0, out_filename.length()-format.length())+"_"+prev_var_type+format;
//     create_or_empty(fname);
//     append_to_file(fname, global_indexes[prev_var_type]+"\n");
//     append_to_file(fname, text);
// }

void convert_output_to_csv(const string &in_filename, const string &out_filename){
    string fname, index, line, word, indexes, text = "";
    string format = out_filename.substr(out_filename.rfind('.'), out_filename.length()); // consigue formato de salida
    bool is_var;
    string var_name = " ";
    string prev_var_name = var_name;
    int size, i;

    fstream file_in(in_filename, ios::in);

    if(file_in.is_open()){
        while(getline(file_in, line) && line.rfind("Variable Name", 0) != 0); // itera hasta conseguir el string de rfind
        while(getline(file_in, line) && line[0] != 'A'){ // extrae variables hasta encontrar la 'A' de "All other variables..."
            stringstream str(line);
            is_var = true;
            while(getline(str, word, ' ')){
                if(word[0] == '\0') continue; // caracter nulo
                if(is_var){
                    var_name = ""; i = 0;
                    while(word[i] > 64 && word[i] < 91) var_name += word[i++]; // consigue nombre de variable de la palabra actual
                    indexes = word.substr(var_name.length(), word.length()); 
                    text += word+",";
                    is_var = false;
                }
                else{
                    text += word.substr(0,word.length()-1)+",";
                }
            }
            size = indexes.length();

            if(var_name != prev_var_name && prev_var_name != " "){
                fname = out_filename.substr(0, out_filename.length()-format.length())+"_"+prev_var_name+format;
                create_or_empty(fname);
                append_to_file(fname, global_indexes[prev_var_name]+"\n");
                append_to_file(fname, text.substr(0, text.rfind('\n')));
                text = text.substr(text.rfind('\n')+1, text.length());  
            }
            prev_var_name = var_name;

            if(var_name == "Y"){ // 'Y' tiene un indice especial
                for(int i = 0; i < size; i+=2){ // se asume size par
                    index = indexes.substr(i,2);
                    if(index[0] == '0') index = index[1];
                    text += (i==size-4? "-":"")+index+(i < size-2? ",":"\n");
                }
            }
            else{ // 'V', 'S', "SST"
                for(int i = 0; i < size; i+=2){ // se asume size par
                    index = indexes.substr(i,2);
                    if(index[0] == '0') index = index[1];
                    text += index+(i < size-2? "," : "\n");
                }
            }
        }
    }

    fname = out_filename.substr(0, out_filename.length()-format.length())+"_"+prev_var_name+format;
    create_or_empty(fname);
    append_to_file(fname, global_indexes[prev_var_name]+"\n");
    append_to_file(fname, text);
}



int main(){
    global_indexes["Y"] = "variable,value,prop,region,zona,sitio,manejo,plantacion,cosecha";
    global_indexes["S"] = "variable,value,prop,region,zona,sitio,manejo,cosecha";
    global_indexes["V"] = "variable,value,prop,region,manejo,producto,cosecha";
    global_indexes["SST"] = "variable,value,prop,region,zona,sitio,manejo,cosecha";

    convert_output_to_csv("../data/1113a.txt", "../out/decoded.csv");

    return 0;
}