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
    // Formato string para indices (e.g. 1 -> "01" / 16 -> "16")
    return (num < 10? "0"+to_string(num) : to_string(num)); 
}

template <typename T> inline string format(const T num) {
    // Formato acotado con _DIGITS decimales
    ostringstream ss;
    ss.precision(_DIGITS);
    ss << num;
    return ss.str();
}

template <typename T> vector<T> get_range(const T prop, const T clase){
    // Rango de edades para propietario p y manejo m
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

template <typename T> inline vector<T> C(const T m){
    // cambio de especie desde manejo m
    if(m == 8) return {9,11,12};
    if(m > 0 && m <= 10) return {m,11,12};
    if(m > 0 && m <= 12) return {1,2,3,4,5,6,7,9,11,12};
    return {};
}



template <typename T, typename value_type> 
void objective_function(string& str_func, string& str_col, string& str_row, map<vector<T>, value_type>& perf){
    // Función Objetivo
    string str_p, str_r, str_z, str_s, str_m, str_k, str_i, str_j;
    string var, coef;
    float v_sum;

    str_row += "FO,N,0\n";

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
                                    var = "Y"+str_p+str_r+str_z+str_s+str_m+str_i+str_j;
                                    coef = format(v_sum);
                                    str_func += coef+" "+var+" + ";
                                    str_col += "FO,"+var+","+coef+"\n";
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



template <typename T, typename value_type> 
void constraint1(string& str_rest, string& str_col, string& str_row, map<vector<T>, value_type>& surf){
    // Restricción de inventario inicial
    string str_p, str_r, str_z, str_s, str_m, str_k, str_i, str_j;
    string str_temp, rest, rhi;

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
                            rest = "AI"+str_p+str_r+str_z+str_s+str_m+str_i;
                            rhi = format(surf[{p,r,z,s,m,i}]);
                            str_temp = rest+": ";
                            for(unsigned short j = max(0, -i+rg[0]); j <= max(0, -i+rg[1]); j++){
                                str_j = index_string(j);
                                str_temp += "Y"+str_p+str_r+str_z+str_s+str_m+str_i+str_j+" + ";
                            }
                            str_temp = str_temp.substr(0, str_temp.length()-3);
                            str_rest += str_temp+" = "+rhi+" ";
                            str_row += rest+",E,"+rhi+"\n";
                        }
                    }
                }
            }
        }
    }
}



void constraint2(string& str_rest, string& str_col, string& str_row){
    // Cuantificación de superficie cosechada
    string str_p, str_r, str_z, str_s, str_m, str_k, str_i, str_j;
    string rest;
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
                        for(unsigned short j = 0; j < rg[1]; j++){ // no se incluye rg[1] pues i debe empezar desde 01
                            str_j = index_string(j);
                            rest = "SC"+str_p+str_r+str_z+str_s+str_m+str_j;
                            str_rest += rest+": ";
                            i_max = (j == 0? I_MAX : abs(min(0, j-rg[1]))); // plantacion sobremadura se va para j = 0
                            for(unsigned short i = abs(min(0, j-rg[0]))+1; i <= i_max; i++){
                                str_i = index_string(i);
                                str_rest += "Y"+str_p+str_r+str_z+str_s+str_m+str_i+str_j+(i != i_max? " + " : "");
                            }
                            str_rest += " - S"+str_p+str_r+str_z+str_s+str_m+str_j+" = 0 ";
                            str_row += rest+",E,0";

                        }
                    }
                }
            }
        }
    }
}



template <typename T, typename value_type> 
void constraint3(string& str_rest, string& str_col, string& str_row, map<vector<T>, value_type>& perf){
    // Cuantificación de volumen  de corta final
    string str_p, str_r, str_z, str_s, str_m, str_k, str_i, str_j;
    string str_temp, rest, var, coef;
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
                                        var = "Y"+str_p+str_r+str_z+str_s+str_m+str_i+str_j;
                                        coef = format(value);
                                        str_temp += coef+" "+var+" + ";
                                        str_col += "VC,"+var+","+coef+"\n";
                                    }
                                }
                            }
                        }
                        if(str_temp.length() > 1){ // restriccion no trivial (0 = 0)
                            rest = "VC"+str_p+str_r+str_m+str_k+str_j;
                            str_temp = str_temp.substr(0,str_temp.length()-3); // extraccion de ' + '
                            str_rest += rest+": "+str_temp;
                            str_rest += " - V"+str_p+str_r+str_m+str_k+str_j+" = 0 ";
                            str_row += rest+",E,0";
                        }
                    }
                }
            }
        }
    }
}



template <typename T, typename value_type> 
void constraint4(string& str_rest, string& str_col, string& str_row, map<vector<T>, value_type>& reconv){
    // Límite de superficie de reconversión
    string str_r, str_z, str_s, str_m, str_k, str_j;
    string rest, rhi;
    float sup_rec;

    for(unsigned short r: R){
        str_r = index_string(r);
        for(unsigned short z: Z){
            str_z = index_string(z);
            for(unsigned short m: M){
                str_m = index_string(m);
                rest = "RET"+str_r+str_z+str_m;
                str_rest += rest+": ";
                for(unsigned short j = 0; j <= 10; j++){
                    str_j = index_string(j);
                    for(unsigned short s: S){
                        str_s = index_string(s);
                        str_rest += "REC01"+str_r+str_z+str_s+str_m+str_j+" + ";
                    }
                }
                sup_rec = reconv[{r,z,m}];
                rhi = format(sup_rec);
                str_rest = str_rest.substr(0, str_rest.length()-3);
                str_rest += " = " + rhi + " ";
                str_row += rest+",E,"+rhi+"\n";
            }
        }
    }
}



void constraint5(string& str_rest, string& str_col, string& str_row){
    // Exclusión superficie de reconversión
    string str_p, str_r, str_z, str_s, str_m, str_k, str_i, str_j;
    string rest;

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
                            rest = "LST"+str_p+str_r+str_z+str_s+str_m+str_j;
                            str_rest += rest+": ";
                            str_rest += "S"+str_p+str_r+str_z+str_s+str_m+str_j+" - ";
                            str_rest += "SST"+str_p+str_r+str_z+str_s+str_m+str_j+" - ";
                            str_rest += "REC"+str_p+str_r+str_z+str_s+str_m+str_j+" = 0 ";
                            str_row += rest+",E,0\n";
                        }
                    }
                }
            }
        }
    }
}



template <typename T, typename value_type> void constraint6(string& str_rest, map<vector<T>, value_type>& supMax){
    // Límite de superficie con cambio de especie
    string str_p, str_r, str_z, str_s, str_m, str_n, str_j, str_c;

    for(unsigned short p: R){
        str_p = index_string(p);
        for(unsigned short r: R){
            str_r = index_string(r);
            for(unsigned short z: Z){
                str_z = index_string(z);
                for(unsigned short s: S){
                    str_s = index_string(s);
                    for(unsigned short m: M){
                        str_m = index_string(m);
                        for(unsigned short c = 1; c <= 7; c++){
                            str_c = index_string(c);
                            str_rest += "TT"+str_p+str_r+str_z+str_s+str_m+str_c+": ";
                            for(unsigned short n: C(c)){
                                for(unsigned short j = 0; j <= 10; j++){
                                    str_rest += "T"+str_p+str_r+str_z+str_s+str_m+str_n+str_j+" + ";
                                }
                            }
                            str_rest = str_rest.substr(0, str_rest.length()-3);
                            str_rest += " <= ????? ";
                        }
                    }
                }
            }
        }
    }
}











// RESTRICCIONES NO IMPLEMENTADAS



template <typename T, typename value_type> void constraint7(string& str_rest, map<vector<T>, value_type>& perf){
    // Reforestación  A (superficie disponible para reforestar con o sin cambio de especie)
    string str_p, str_r, str_z, str_s, str_m, str_k, str_i, str_j, str_n;

    for(unsigned short p: R){
        str_p = index_string(p);
        for(unsigned short r: R){
            str_r = index_string(r);
            for(unsigned short z: Z){
                str_z = index_string(z);
                for(unsigned short s: S){
                    str_s = index_string(s);
                    for(unsigned short m: M){
                        str_m = index_string(m);
                        for(unsigned short j = 0; j <= 35; j++){
                            str_j = index_string(j);
                            str_rest += "RFA"+str_p+str_r+str_z+str_s+str_m+str_j+": ";
                            str_rest += "SST"+str_p+str_r+str_z+str_s+str_m+str_j+" - ";
                            for(unsigned short n: C(m)){
                                str_n = index_string(n);
                                str_rest += "T"+str_p+str_r+str_s+str_m+str_n+str_j+" + ";
                            }
                            str_rest = str_rest.substr(0, str_rest.length()-3);
                            str_rest += " = 0";
                        }
                    }
                }
            }
        }
    }
}

template <typename T, typename value_type> void constraint8(string& str_rest, map<vector<T>, value_type>& perf){
    // Reforestación  B (superficie a reforestar con una especie igual o distinta a la anterior)
    string str_p, str_r, str_z, str_s, str_m, str_k, str_i, str_j, str_l;
    bool use_l, delete_plus;
    unsigned short k_max;
    vector<unsigned short> rg;

    for(unsigned short p: R){
        str_p = index_string(p);
        for(unsigned short r: R){
            str_r = index_string(r);
            for(unsigned short z: Z){
                str_z = index_string(z);
                for(unsigned short s: S){
                    str_s = index_string(s);
                    for(unsigned short m: M){
                        str_m = index_string(m);
                        rg = get_range<unsigned short>(p, m); // rango de edades
                        for(unsigned short j = 0; j <= 35; j++){
                            str_j = index_string(j);
                            str_rest += "RFB"+str_p+str_r+str_z+str_s+str_m+str_j+": ";
                            delete_plus = false;
                            for(unsigned short l = 0; l <= 12; l++){
                                str_l = index_string(l);
                                use_l = false;
                                for(unsigned short ii: C(l)){
                                    if(m == ii){
                                        use_l = true;
                                        delete_plus = true;
                                        break;
                                    }
                                }
                                if(use_l){
                                    str_rest += "T"+str_p+str_r+str_z+str_s+str_l+str_m+str_j+" + ";
                                }
                            }
                            if(delete_plus){
                                str_rest = str_rest.substr(0, str_rest.length()-3);
                            }
                            str_rest += "F"+str_p+str_r+str_z+str_s+str_m+str_j;
                            k_max = abs(min(0, j-rg[1]));
                            for(unsigned short k = abs(min(0, j-rg[0]))+1; k <= k_max; k++){
                                str_k = index_string(k);
                                str_rest += " - X"+str_p+str_r+str_z+str_s+str_m+str_j+str_k;
                            }
                            str_rest += " - W"+str_p+str_r+str_z+str_s+str_m+str_j+" = 0 ";
                        }
                    }
                }
            }
        }
    }
}

template <typename T, typename value_type> void constraint9(string& str_rest, map<vector<T>, value_type>& perf){
    // Cuantificación de volumen de raleos
    string str_p, str_r, str_z, str_s, str_m, str_k, str_i, str_j, str_n;

    for(unsigned short p: R){
        str_p = index_string(p);
        for(unsigned short r: R){
            str_r = index_string(r);
            for(unsigned short m: M){
                str_m = index_string(m);
                for(unsigned short j = 0; j <= 35; j++){
                    str_j = index_string(j);
                    str_rest += "RT"+str_p+str_r+str_m+str_j+": ";
                    for(unsigned short z: Z){
                        for(unsigned short s: S){
                            
                        }
                    }
                }
            }
        }
    }
}