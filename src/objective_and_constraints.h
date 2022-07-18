#include <vector>
#include <map>
#include <string>
#include <sstream>

using namespace std;

// DEFINICION DE CONJUNTOS

#define _SETS
#define P {1,2,3,4} // propietario
#define R {5,6,7,8,9,10,13,14} // region administrativa
#define Z {1,2,4,5,6,7,9} // zona de crecimiento
#define S {1,2,3,4} // clases de sitio
#define M {1,2,3,7,8,9,10,11,12} // esquemas de manejo
#define K {1,2,3,4,5,6,7,8,9,10,11,12,13} // producto
#define I_MAX 30 // año de plantacion (-)

#define _DIGITS 4



template <typename T> inline string index_string(const T num){
    return (num < 10? "0"+to_string(num) : to_string(num)); 
}

template <typename T> inline string format(const T num) {
    ostringstream ss;
    ss.precision(_DIGITS);
    ss << num;
    return ss.str();
}

template <typename T> vector<T> get_range(const T prop, const T clase){
    // Rango de edades para propietario p y clase m
    if(clase > 3 && clase < 7) return {0,0};
    if(prop < 3 && clase == 1) return {18, 23};
    if(prop < 5 && clase == 1) return {18, 22};
    if(prop < 3 && clase < 4) return {20,23};
    if(prop < 5 && clase < 4) return {18,23};
    if(prop < 3 && clase < 11) return {22, 26};
    if(prop < 5 && clase < 11) return {22, 25};
    if(prop == 1 && clase == 11) return {11, 18};
    if(prop == 2 && clase == 11) return {10, 18};
    if(prop < 5 && clase == 11) return {9,12};
    if(prop < 3 && clase == 12) return {13,18};
    if(prop < 5 && clase == 12) return {11,18};
    return {0,0};
}



template <typename T, typename value_type> 
void objective_function(string& str_func, map<vector<T>, value_type>& perf){
    // Función Objetivo
    string str_p, str_r, str_z, str_s, str_m, str_k, str_i, str_j;
    float v_sum;

    for(unsigned short p: P){
        str_p = index_string(p);
        for(unsigned short r: R){
            str_r = index_string(r);
            for(unsigned short z: Z){
                str_z = index_string(z);
                for(unsigned short s: S){
                    str_s = index_string(s);
                    for(unsigned short m: M){
                        str_m = index_string(m);
                        vector<unsigned short> rg = get_range<unsigned short>(p, m); // rango de edades
                        for(unsigned short i = I_MAX; i > 0; --i){
                            str_i = index_string(i);
                            for(unsigned short j = max(0, -i+rg[0]); j <= max(0, -i+rg[1]); j++){
                                str_j = index_string(j);
                                v_sum = 0.F;
                                for(unsigned short k: K){
                                    v_sum += perf[{k,z,s,m,(unsigned short)(i+j)}];
                                }
                                if(v_sum > 1e-3){ // multiplicador != 0
                                    str_func += format(v_sum)+" Y"+str_p+str_r+str_z+str_s+str_m+str_i+str_j+" + ";
                                }
                            }
                        }
                    }
                }
            }
        }
    }
    str_func = str_func.substr(0, str_func.length()-3); // extraccion de ' + '
}



template <typename T, typename value_type> void constraint1(string& str_rest, map<vector<T>, value_type>& surf){
    // Restricción de inventario inicial
    string str_p, str_r, str_z, str_s, str_m, str_k, str_i, str_j;
    string str_temp;

    for(unsigned short p: P){
        str_p = index_string(p);
        for(unsigned short r: R){
            str_r = index_string(r);
            for(unsigned short z: Z){
                str_z = index_string(z);
                for(unsigned short s: S){
                    str_s = index_string(s);
                    for(unsigned short m: M){
                        str_m = index_string(m);
                        vector<unsigned short> rg = get_range<unsigned short>(p, m); // rango de edades
                        for(unsigned short i = I_MAX; i > 0; --i){
                            str_i = index_string(i);
                            str_temp = "AI"+str_p+str_r+str_z+str_s+str_m+str_i+": ";
                            for(unsigned short j = max(0, -i+rg[0]); j <= max(0, -i+rg[1]); j++){
                                str_j = index_string(j);
                                str_temp += "Y"+str_p+str_r+str_z+str_s+str_m+str_i+str_j+" + ";
                            }
                            str_temp = str_temp.substr(0, str_temp.length()-3);
                            str_rest += str_temp+" = "+format(surf[{p,r,z,s,m,i}])+" ";
                            
                        }
                    }
                }
            }
        }
    }
}



template <typename T, typename value_type> void constraint2(string& str_rest, map<vector<T>, value_type>& surf){
    // Cuantificación de superficie cosechada
    string str_p, str_r, str_z, str_s, str_m, str_k, str_i, str_j;
    unsigned short i_max;

    for(unsigned short p: P){
        str_p = index_string(p);
        for(unsigned short r: R){
            str_r = index_string(r);
            for(unsigned short z: Z){
                str_z = index_string(z);
                for(unsigned short s: S){
                    str_s = index_string(s);
                    for(unsigned short m: M){
                        str_m = index_string(m);
                        vector<unsigned short> rg = get_range<unsigned short>(p, m); // rango de edades
                        for(unsigned short j = 0; j < rg[1]; j++){
                            str_j = index_string(j);
                            str_rest += "SC"+str_p+str_r+str_z+str_s+str_m+str_j+") ";
                            i_max = abs(min(0, j-rg[1]));
                            for(unsigned short i = abs(min(0, j-rg[0]))+1; i <= i_max; i++){
                                str_i = index_string(i);
                                str_rest += "Y"+str_p+str_r+str_z+str_s+str_m+str_i+str_j+(i != i_max? " + " : "");
                            }
                            str_rest += " - ????? = 0 ";

                        }
                    }
                }
            }
        }
    }
}



template <typename T, typename value_type> void constraint3(string& str_rest, map<vector<T>, value_type>& perf){
    // Cuantificación de volumen  de corta final
    string str_p, str_r, str_z, str_s, str_m, str_k, str_i, str_j;
    string str_temp;
    unsigned short i_max;
    float value;

    for(unsigned short p: P){
        str_p = index_string(p);
        for(unsigned short r: R){
            str_r = index_string(r);
            for(unsigned short m: M){
                str_m = index_string(m);
                vector<unsigned short> rg = get_range<unsigned short>(p, m); // rango de edades
                for(unsigned short k: K){
                    str_k = index_string(k);
                    for(unsigned short j = 0; j < rg[1]; j++){
                        str_j = index_string(j);
                        str_temp = "";
                        i_max = abs(min(0, j-rg[1]));
                        for(unsigned short z: Z){
                            str_z = index_string(z);
                            for(unsigned short s: S){
                                str_s = index_string(s);
                                for(unsigned short i = abs(min(0, j-rg[0]))+1; i <= i_max; i++){
                                    value = perf[{k,z,s,m,(unsigned short)(i+j)}];
                                    if(value > 1e-3){
                                        str_i = index_string(i);
                                        str_temp += format(value)+" ";
                                        str_temp += "Y"+str_p+str_r+str_z+str_s+str_m+str_i+str_j+" + ";
                                    }
                                }
                            }
                        }
                        if(str_temp.length() > 1){ // restriccion no trivial (0 = 0)
                            str_temp = str_temp.substr(0,str_temp.length()-3); // extraccion de ' + '
                            str_rest += "VC"+str_p+str_r+str_m+str_k+str_j+") "+str_temp+" - ????? = 0 ";
                        }
                    }
                }
            }
        }
    }
}

template <typename T, typename value_type> void constraint4(string& str_rest, map<vector<T>, value_type>& perf){
    // Cuantificación de volumen  de corta final
    string str_p, str_r, str_z, str_s, str_m, str_k, str_i, str_j;

    for(unsigned short p: P){
        str_p = index_string(p);
        for(unsigned short r: R){
            str_r = index_string(r);
            for(unsigned short z: Z){
                str_z = index_string(z);
                for(unsigned short s: S){
                    str_s = index_string(s);
                    for(unsigned short m: M){
                        str_m = index_string(m);
                        vector<unsigned short> rg = get_range<unsigned short>(p, m); // rango de edades
                        for(unsigned short j = 0; j < rg[1]; j++){
                            str_j = index_string(j);
                            str_rest += "LST"+str_p+str_r+str_z+str_s+str_m+str_j+") ";
                            str_rest += "????? - ????? - ????? = 0 ";
                        }
                    }
                }
            }
        }
    }
}