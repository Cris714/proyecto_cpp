#include <iostream>
#include <vector>
#include <map>
#include <unordered_map>
#include <chrono>

#include "file_manager.h"

template <class T>
inline void hash_combine(std::size_t& seed, T const& v)
{
    seed ^= std::hash<T>()(v) + 0x9e3779b9 + (seed << 6) + (seed >> 2);
}

namespace std
{
    template<typename T>
    struct hash<vector<T>>
    {
        typedef vector<T> argument_type;
        typedef std::size_t result_type;
        result_type operator()(argument_type const& in) const
        {
            size_t size = in.size();
            size_t seed = 0;
            for (size_t i = 0; i < size; i++)
                //Combine the hash of the current vector with the hashes of the previous ones
                hash_combine(seed, in[i]);
            return seed;
        }
    };
}


using namespace std;

int main(){
    using namespace std::chrono;

    string path, line = "", str_i, str_j, str_k, str_z, str_r;
    high_resolution_clock::time_point t1;
    high_resolution_clock::time_point t2;
    duration<double> time_span;

    // USANDO HASHMAP
    unordered_map<vector<int>, int> fast;
    unordered_map<vector<int>, int> fast_areas;
    path = "./output3.txt";

    t1 = high_resolution_clock::now();
    cout << "Generando hashmap..." << endl;
    
    load_file_to_hashmap<int>("./sample.txt", fast);
    load_file_to_hashmap<int>("./areas.txt", fast_areas);

    t2 = high_resolution_clock::now();
    time_span = duration_cast<duration<double>>(t2 - t1);
    std::cout << "Elapsed time: " << time_span.count() << "s" << endl;

    t1 = high_resolution_clock::now();
    cout << "Generando funcion..." << endl;

    for(int i = 1; i <= 100; i++){
        str_i = to_string(i);
        for(int j = 1; j <= 20; j++){
            str_j = to_string(j);
            for(int k = 1; k <= 40; k++){
                str_k = to_string(k);
                for(int z = 1; z <= 10; z++){
                    str_z = to_string(z);
                    for(int r = 1; r <= 10; r++){
                        str_r = to_string(r);
                        line += to_string(fast[{i,j,k,z,r}]) + " X" + (i < 10? "0"+str_i : str_i) + (j < 10? "0"+str_j : str_j)
                                + (k < 10? "0"+str_k : str_k) + (j < 10? "0"+str_z : str_z) + (r < 10? "0"+str_r : str_r);
                        for(int c = 0; c < 10; c++) fast[{i,j,k,z,r}];
                        
                        if(i != 10 || j != 3 || k != 40 || z != 10 || r != 2){
                            line += " + ";
                        }
                    }
                }
            }
        }
    }

    t2 = high_resolution_clock::now();
    time_span = duration_cast<duration<double>>(t2 - t1);
    std::cout << "Elapsed time: " << time_span.count() << "s" << endl;

    fast.clear();

    create_or_empty(path);
    append_to_file(path, "Max\n");
    append_to_file(path, line);



    // USANDO MAP
    map<vector<int>, int> values; 
    map<vector<int>, int> areas;
    path = "./output.txt";
    line = "";


    t1 = high_resolution_clock::now();
    cout << "Generando mapa..." << endl;
    
    load_file_to_map<int>("./sample.txt", values);
    load_file_to_map<int>("./areas.txt", areas);

    t2 = high_resolution_clock::now();
    time_span = duration_cast<duration<double>>(t2 - t1);
    std::cout << "Elapsed time: " << time_span.count() << "s" << endl;


    create_or_empty(path);
    append_to_file(path, "Max\n");


    // Funcion 

    t1 = high_resolution_clock::now();
    cout << "Generando funcion..." << endl;

    for(int i = 1; i <= 100; i++){
        str_i = to_string(i);
        for(int j = 1; j <= 20; j++){
            str_j = to_string(j);
            for(int k = 1; k <= 40; k++){
                str_k = to_string(k);
                for(int z = 1; z <= 10; z++){
                    str_z = to_string(z);
                    for(int r = 1; r <= 10; r++){
                        str_r = to_string(r);
                        line += to_string(values[{i,j,k,z,r}]) + " X" + (i < 10? "0"+str_i : str_i) + (j < 10? "0"+str_j : str_j)
                                + (k < 10? "0"+str_k : str_k) + (j < 10? "0"+str_z : str_z) + (r < 10? "0"+str_r : str_r);
                        for(int c = 0; c < 10; c++) values[{i,j,k,z,r}];
                        
                        if(i != 10 || j != 3 || k != 40 || z != 10 || r != 2){
                            line += " + ";
                        }
                    }
                }
            }
        }
    }

    t2 = high_resolution_clock::now();
    time_span = duration_cast<duration<double>>(t2 - t1);
    std::cout << "Elapsed time: " << time_span.count() << "s" << endl;
/*
    for(int i = 1; i <= 10; i++){
        str_i = to_string(i);
        for(int j = 1; j <= 2; j++){
            str_j = to_string(j);
            line += to_string(values[{i,j}]) + " X" + (i < 10? "0"+str_i : str_i) + (j < 10? "0"+str_j : str_j);
            if(i != 10 || j != 2){
                line += " + ";
            }
        }
    }
*/
    append_to_file(path, line+"\n");

    append_to_file(path, "Subject to\n");

    for(int i = 1; i <= 10; i++){
        line = "";
        str_i = to_string(i);
        line += "SUP"+(i < 10? "0"+str_i : str_i)+") ";
        for(int j = 1; j <= 2; j++){
            str_j = to_string(j);
            line += "X" + (i < 10? "0"+str_i : str_i) + (j < 10? "0"+str_j : str_j);
            if(j != 2){
                line += " + ";
            }
        }
        line += " <= " + to_string(areas[{i}]) + (i != 10? "\n" : "");
        append_to_file(path, line);
    }



    // USANDO TABLE

    t1 = high_resolution_clock::now();
    cout << "Generando tabla..." << endl;
    
    Table<int> data, areas_table;
    load_file_to_table<int>("./sample.txt", data);
    load_file_to_table<int>("./areas.txt", areas_table);

    t2 = high_resolution_clock::now();
    time_span = duration_cast<duration<double>>(t2 - t1);
    std::cout << "Elapsed time: " << time_span.count() << "s" << endl;

    data.add_param("i", 1, 100);
    data.add_param("j", 1, 20);
    data.add_param("k", 1, 40);
    data.add_param("z", 1, 10);
    data.add_param("r", 1, 2);

    t1 = high_resolution_clock::now();
    cout << "Generando funcion..." << endl;

    line = "";

    for(int i = 1; i <= 100; i++){
        str_i = to_string(i);
        for(int j = 1; j <= 20; j++){
            str_j = to_string(j);
            for(int k = 1; k <= 40; k++){
                str_k = to_string(k);
                for(int z = 1; z <= 10; z++){
                    str_z = to_string(z);
                    for(int r = 1; r <= 10; r++){
                        str_r = to_string(r);
                        line += to_string(data[{i,j,k,z,r}]) + " X" + (i < 10? "0"+str_i : str_i) + (j < 10? "0"+str_j : str_j)
                                + (k < 10? "0"+str_k : str_k) + (j < 10? "0"+str_z : str_z) + (r < 10? "0"+str_r : str_r);
                        
                        for(int c = 0; c < 10; c++) data[{i,j,k,z,r}];
                        
                        if(i != 10 || j != 3 || k != 40 || z != 10 || r != 2){
                            line += " + ";
                        }
                    }
                }
            }
        }
    }

    t2 = high_resolution_clock::now();
    time_span = duration_cast<duration<double>>(t2 - t1);
    std::cout << "Elapsed time: " << time_span.count() << "s" << endl;

    path = "./output2.txt";
    create_or_empty(path);
    append_to_file(path, "Max\n");
    append_to_file(path, line);

    return 0;
}
