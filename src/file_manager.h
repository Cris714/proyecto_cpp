#include <iostream>
#include <fstream>
#include <sstream>
#include <map>
#include <vector>

#define MAX_WIDTH 70 

using namespace std;


template <typename key_type, typename value_type> 
void load_file_to_map(const string &path, map<vector<key_type>, value_type> &values){
    vector<key_type> key;
    string line, word;
    fstream file(path, ios::in);

    if(file.is_open()){
        getline(file, line); // salta los nombre de indice
        while(getline(file, line)){
            stringstream str(line);
            key.clear();
            while(getline(str, word, ',')){
                key.push_back(abs(stoi(word)));
            }
            key.pop_back();
            values[key] = stod(word);
        }
    }
}


void create_or_empty(const string& path){
    ofstream file(path);
    file.close();
}


void append_to_file(const string& path, const string& text, const char flag='n'){
    /*
     - flags:
    'n' <- (default) solo escribe
    'f' <- función objetivo
    'r' <- restricciones (solo si contiene variables con nombres de 1 letra)
    'a' <- restricciones (para cualquier restricción, pero más lento)
    */
    ofstream file(path, ios::app);
    string line = "";
    const int text_length = text.length();

    if(flag == 'f'){
        string temp_str = "";
        int line_width = 0;

        for(int i = 0; i < text_length; i++){
            if(text[i] != ' '){
                temp_str += text[i];
            }
            else if(line_width + temp_str.length() + 1 < MAX_WIDTH){
                line += (line_width == 0? "" : " ")+temp_str;
                line_width += (line_width == 0? 0 : 1)+temp_str.length();
                temp_str = "";
            }
            else{
                line += " \n"+temp_str;
                line_width = temp_str.length();
                temp_str = "";
            }
        }
        file << line;
        file.close();
    }
    else if(flag == 'r'){
        string temp_str = "";
        char char_zero = text[0];
        int line_width = 0;

        for(int i = 0; i < text_length; i++){
            if(text[i] == char_zero && text[i+1] > 64){ // controla el salto de linea solo si empieza con char_zero y le sigue una letra
                line += "\n";
                temp_str = "";
                line_width = 0;
            }
            
            if(text[i] != ' '){
                temp_str += text[i];
            }
            else if(line_width + temp_str.length() + 1 < MAX_WIDTH){
                line += (line_width == 0? "" : " ")+temp_str;
                line_width += (line_width == 0? 0 : 1)+temp_str.length();
                temp_str = "";
            }
            else{
                line += " \n"+temp_str;
                line_width = temp_str.length();
                temp_str = "";
            }
        }
        file << line;
        file.close();
    }
    else if(flag == 'a'){
        string temp_str = "";
        string temp_var_name = "", rvar_name = "";
        int i = 0, line_width = 0;

        // consigue el nombre de la variable de restricción
        while(text[i] > 64 && text[i] < 91){
            rvar_name += text[i++];
        }

        for(int i = 0; i < text_length; i++){
            if(text[i] > 64 && text[i] < 91){ // busca y construye nombres de variable
                temp_var_name += text[i];
            }
            else{
                temp_var_name = "";
            }

            if(temp_var_name == rvar_name){ // controla el salto de linea solo si se encuentra rvar_name
                line += "\n";
                temp_str = rvar_name;
                line_width = 0;
            }
            
            else if(text[i] != ' '){
                temp_str += text[i];
            }
            else if(line_width + temp_str.length() + 1 < MAX_WIDTH){
                line += (line_width == 0? "" : " ")+temp_str;
                line_width += (line_width == 0? 0 : 1)+temp_str.length();
                temp_str = "";
            }
            else{
                line += " \n"+temp_str;
                line_width = temp_str.length();
                temp_str = "";
            }
        }
        file << line;
        file.close();
    }
    else if(flag == 'n'){
        file << text;
        file.close();
    }
}

