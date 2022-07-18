#include <iostream>
#include <fstream>
#include <sstream>

using namespace std;



void convert_output_to_csv(const string &in_filename, const string &out_filename){
    string line, word, indexes, text = "";
    bool is_var;
    char var_type;
    int size;

    fstream file_in(in_filename, ios::in);
    ofstream file_out(out_filename);

    if(file_in.is_open()){
        while(getline(file_in, line) && line.rfind("Variable Name", 0) != 0); // itera hasta conseguir el string de rfind
        while(getline(file_in, line) && line[0] != 'A'){
            stringstream str(line);
            is_var = true;
            while(getline(str, word, ' ')){
                if(word[0] == '\0') continue;
                if(is_var){
                    var_type = word[0];
                    indexes = word.substr(1, word.length());
                    text += word+",";
                    is_var = false;
                }
                else{
                    text += word.substr(0,word.length()-1)+",";
                }
            }
            size = indexes.length();
            switch(var_type){
                case 'Y':
                    for(int i = 0; i < size; i+=2){ // se asume size par
                        text += (i==size-4? "-":"")+indexes.substr(i,2)+(i < size-2? "," : "\n");
                    }
                    break;
                default:
                    break;
            }
        }
    }
    file_out << "variable,value,prop,region,zona,sitio,manejo,plantacion,cosecha\n";
    file_out << text;
    file_out.close();
}

/**
 * @brief PENDIENTE
 * 
 * @param in_filename 
 * @param out_filename 
 * @param indexes string con etiquetas de indices (e.g. "przsmij")
 */
void convert_output_to_csv(const string &in_filename, const string &out_filename, const string &indexes){


}


int main(){
    convert_output_to_csv("../data/salida0629d.txt", "../out/decoded.csv");
    return 0;
}