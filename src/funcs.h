#include <vector>
#include <map>
#include <string>
#include <sstream>

using namespace std;

// DEFINICION DE CONJUNTOS

#define P {1,2,3,4} // propietario
#define R {5,6,7,8,9,10,13,14,16} // region administrativa
#define Z {1,2,4,5,6,7,9} // zona de crecimiento
#define S {1,2,3,4} // clases de sitio
#define M {1,2,3,4,5,6,9,10,11} // esquemas de manejo
#define K {1,2,3,4,5,6,7} // producto
#define HP 45 // horizonte de planificacion

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

template <typename T> vector<T> get_range(const T p, const T m){
    // Rango de edades para propietario p y manejo m
    if (m < 1) return { 0, 0 };
    if (m < 3) return { 18, 23 };
    if (m < 5) return { 20, 24 };
    if (m < 7) return { 22, 26 };
    if (m < 10) return { 12, 18 };
    if (m < 12) return { 13, 18 };
    return {0,0};
}

template <typename T> vector<T> get_range_over(const T p, const T m) {
    // Rango de edades de plantaciones sobremaduras para propietario p y manejo m
    if (m < 1) return { 0, 0 };
    if (m < 3) return { 24, 35 };
    if (m < 5) return { 25, 35 };
    if (m < 7) return { 27, 35 };
    if (m < 10) return { 19, 25 };
    if (m < 12) return { 19, 30 };
    return { 0,0 };
}



template <typename T, typename value_type> 
void objective_function(
    string& str_func, 
    string& str_col, 
    string& str_row, 
    map<vector<T>, value_type> &rendimientos,
    map<vector<T>, value_type> &factor
    ){
    // Función Objetivo
    string str_p, str_r, str_z, str_s, str_m, str_k, str_i, str_j;
    string var, coef;
    float v_sum, fact;
    unsigned short age, j_max;

    vector<unsigned short> rg, rgo;

    str_row += "FO,N,0\n";

    for (unsigned short p : P) {
        str_p = index_string(p);
        for (unsigned short r : R) {
            str_r = index_string(r);
            for (unsigned short z : Z) {
                str_z = index_string(z);
                for (unsigned short s : S) {
                    str_s = index_string(s);
                    for (unsigned short m : M) {
                        str_m = index_string(m);

                        rg = get_range<unsigned short>(p, m); // rango de edades
                        rgo = get_range_over<unsigned short>(p, m); // rango de edades p. sobremaduras

                        for (short i = -rgo[1]+1; i <= HP - rg[0]; i++) {
                            str_i = index_string(i < 0 ? -i : i);
                            j_max = (unsigned short)min(HP, i + (short)(i < 0 ? rgo[1]-1 : rg[1]));
                            for (unsigned short j = max(0, (int)i); j <= j_max; j++) {
                                str_j = index_string(j);

                                v_sum = 0.F;

                                if (i < 0) { // para las variables Y (se añade factor correspondiente)
                                    fact = factor[{p, r, z, s, m, (unsigned short)(-i)}];

                                    if (fact > 1e-3) { // factor > ~0
                                        for (unsigned short k : K) {
                                            age = (i < 0 ? (unsigned short)(-i) + j : j - (unsigned short)i);
                                            if (m <= 6) age = min((unsigned short)30, age);
                                            if (m >= 9) age = min((unsigned short)20, age);
                                            v_sum += rendimientos[{k, z, s, m, age}];
                                        }
                                    }
                                    else continue; // 

                                    coef = format(v_sum * fact);
                                }
                                else { // para las variables X
                                    for (unsigned short k : K) {
                                        age = (i < 0 ? (unsigned short)(-i) + j : j - (unsigned short)i);
                                        v_sum += rendimientos[{k, z, s, m, age}];
                                    }

                                    coef = format(v_sum);
                                }

                                if (v_sum > 1e-3) { // multiplicador > ~0
                                    var = (i < 0 ? "Y" : "X") + str_p + str_r + str_z + str_s + str_m + str_i + str_j;
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

    // variables Rkprmj
    for (unsigned short k = 4; k <= 7; k++) {
        str_k = index_string(k);
        for (unsigned short p : P) {
            str_p = index_string(p);
            for (unsigned short r : R) {
                str_r = index_string(r);
                for (unsigned short m : M) {
                    str_m = index_string(m);
                    for (unsigned short j = 0; j <= HP; j++) {
                        str_j = index_string(j);

                        str_func += "R" + str_k + str_p + str_r + str_m + str_j + " + ";
                    }
                }
            }
        }
    }

    str_func = str_func.substr(0, str_func.length()-3); // extraccion de ' + '
}



template <typename T, typename value_type> 
void constraint1(
    string& str_rest, 
    string& str_col, 
    string& str_row, 
    map<vector<T>, value_type>& superficie
){
    // Restricción de inventario inicial
    string str_p, str_r, str_z, str_s, str_m, str_i, str_j;
    string str_temp, rest, rhi;

    vector<unsigned short> rg, rgo;

    for (unsigned short p : P) {
        str_p = index_string(p);
        for (unsigned short r : R) {
            str_r = index_string(r);
            for (unsigned short z : Z) {
                str_z = index_string(z);
                for (unsigned short s : S) {
                    str_s = index_string(s);
                    for (unsigned short m : M) {
                        str_m = index_string(m);
                        rg = get_range<unsigned short>(p, m); // rango de edades
                        rgo = get_range_over<unsigned short>(p, m); // rango de edades p. sobremaduras
                        for (short i = -rgo[1]; i <= -1; i++) {
                            str_i = index_string(-i);
                            rest = "AI" + str_p + str_r + str_z + str_s + str_m + str_i;
                            rhi = format(superficie[{p, r, z, s, m, (unsigned short)(-i)}]);
                            str_temp = rest+": ";
                            for(unsigned short j = max(0, i+rg[0]); j < max(0, i+rgo[1]); j++){
                                str_j = index_string(j);
                                str_temp += "Y" + str_p + str_r + str_z + str_s + str_m + str_i + str_j + " + ";
                            }
                            str_temp = str_temp.substr(0, str_temp.length()-3);
                            str_rest += str_temp + " <= " + rhi + " ";
                            str_row += rest + ",L," + rhi + "\n";
                        }
                    }
                }
            }
        }
    }
}



void constraint2(
    string& str_rest, 
    string& str_col, 
    string& str_row
){
    // Cuantificación de superficie cosechada
    string str_p, str_r, str_z, str_s, str_m, str_k, str_i, str_j;
    string rest;
    short i_max, i_min;

    vector<unsigned short> rg, rgo;

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

                        rg = get_range<unsigned short>(p, m); // rango de edades
                        rgo = get_range_over<unsigned short>(p, m); // rango de edades p. sobremaduras

                        for(unsigned short j = 0; j <= HP; j++){ 
                            str_j = index_string(j);

                            rest = "SC" + str_p + str_r + str_z + str_s + str_m + str_j;
                            str_rest += rest+": ";

                            i_min = (short)j - (short)(j <= rg[1] ? rgo[1]-1 : rg[1]);
                            //if (i_min < 0) i_min = max(-30, (short)j - (short)rgo[1]); // admite plantaciones sobremaduras para Y
                            i_max = (short)j - (short)rg[0];

                            for(short i = i_min; i <= i_max; i++){
                                str_i = index_string(i < 0 ? -i : i);
                                str_rest += (i < 0 ? "Y" : "X") + str_p + str_r + str_z + str_s + \
                                    str_m + str_i + str_j + (i != i_max ? " + " : "");
                            }
                            str_rest += " - S" + str_p + str_r + str_z + str_s + str_m + str_j + " = 0 ";
                            str_row += rest + ",E,0";

                        }
                    }
                }
            }
        }
    }
}



template <typename T, typename value_type> 
void constraint3(
    string& str_rest, 
    string& str_col, 
    string& str_row, 
    map<vector<T>, value_type>& rendimientos,
    map<vector<T>, value_type>& factor
){
    // Cuantificación de volumen  de corta final
    string str_p, str_r, str_z, str_s, str_m, str_k, str_i, str_j;
    string str_temp, rest, var, coef;
    short i_max, i_min;
    unsigned short age;
    float value, fact;

    vector<unsigned short> rg, rgo;

    for (unsigned short p : P) {
        str_p = index_string(p);
        for (unsigned short r : R) {
            str_r = index_string(r);
            for (unsigned short m : M) {
                str_m = index_string(m);

                rg = get_range<unsigned short>(p, m); // rango de edades
                rgo = get_range_over<unsigned short>(p, m); // rango de edades p. sobremaduras

                for (unsigned short k : K) {
                    str_k = index_string(k);
                    for (unsigned short j = 0; j <= HP; j++) {
                        str_j = index_string(j);

                        str_temp = "";
                        i_min = (short)j - (short)(j <= rg[1] ? rgo[1]-1 : rg[1]);
                        i_max = (short)j - (short)rg[0];

                        for (unsigned short z : Z) {
                            str_z = index_string(z);
                            for (unsigned short s : S){
                                str_s = index_string(s);
                                for (short i = i_min; i <= i_max; i++){
                                    fact = (i < 0 ? factor[{p, r, z, s, m, (unsigned short)(-i)}] : 0.F);

                                    age = (i < 0 ? (unsigned short)(-i) + j : j - (unsigned short)i);
                                    if (m <= 6) age = min((unsigned short)30, age);
                                    if (m >= 9) age = min((unsigned short)20, age);
                                    value = rendimientos[{k, z, s, m, age}];

                                    if (fact > 1e-3 && value > 1e-3) {
                                        str_i = index_string(i < 0 ? -i : i);
                                        var = (i < 0 ? "Y" : "X") + str_p + str_r + str_z + str_s + str_m + str_i + str_j;
                                        coef = format(fact * value);
                                        str_temp += coef + " " + var + " + ";
                                        str_col += "VC," + var + "," + coef + "\n";
                                    }
                                }
                            }
                        }
                        if(str_temp.length() > 1){ // restriccion no trivial (0 = 0)
                            rest = "VC" + str_p + str_r + str_m + str_k + str_j;
                            str_temp = str_temp.substr(0, str_temp.length() - 3); // extraccion de ' + '
                            str_rest += rest + ": " + str_temp;
                            str_rest += " - V" + str_p + str_r + str_m + str_k + str_j + " = 0 ";
                            str_row += rest + ",E,0";
                        }
                    }
                }
            }
        }
    }
}



template <typename T, typename value_type> 
void constraint4(
    string& str_rest, 
    string& str_col, 
    string& str_row, 
    map<vector<T>, value_type>& reconversion
){
    // Límite de superficie de reconversión
    string str_p, str_r, str_z, str_s, str_m, str_k, str_j;
    string rest, rhi;
    float value;
    for (unsigned short p : P) {
        str_p = index_string(p);
        for (unsigned short r : R) {
            str_r = index_string(r);
            for (unsigned short z : Z) {
                str_z = index_string(z);
                for (unsigned short s : S) {
                    str_s = index_string(s);
                    for (unsigned short m : M){
                        str_m = index_string(m);

                        rest = "RET" + str_p + str_r + str_z + str_s + str_m;
                        str_rest += rest + ": ";

                        for (unsigned short j = 0; j <= 10; j++){
                            str_j = index_string(j);
                            str_rest += "REC" + str_p + str_r + str_z + str_s + str_m + str_j + \
                                (j != 10 ? " + " : "");
                        }

                        value = reconversion[{p, r, z, s, m}];
                        rhi = format(value);
                        str_rest += " = " + rhi + " ";
                        str_row += rest + ",E," + rhi + "\n";
                    }
                }
            }
        }
    }
}



void constraint5(
    string& str_rest, 
    string& str_col, 
    string& str_row
){
    // Exclusión superficie de reconversión
    string str_p, str_r, str_z, str_s, str_m, str_k, str_i, str_j;
    string rest;

    for (unsigned short p : P) {
        str_p = index_string(p);
        for (unsigned short r : R) {
            str_r = index_string(r);
            for (unsigned short z : Z) {
                str_z = index_string(z);
                for (unsigned short s : S) {
                    str_s = index_string(s);
                    for (unsigned short m : M) {
                        str_m = index_string(m);
                        for (unsigned short j = 0; j <= HP; j++){
                            str_j = index_string(j);

                            rest = "LST" + str_p + str_r + str_z + str_s + str_m + str_j;
                            str_rest += rest + ": ";
                            str_rest += "S" + str_p + str_r + str_z + str_s + str_m + str_j;
                            str_rest += " - SST" + str_p + str_r + str_z + str_s + str_m + str_j;
                            if (j <= 10) str_rest += " - REC" + str_p + str_r + str_z + str_s + str_m + str_j;
                            str_rest += " = 0 ";
                            str_row += rest + ",E,0\n";
                        }
                    }
                }
            }
        }
    }
}



template <typename T, typename value_type> void constraint6(
    string &str_rest, 
    string &str_col,
    string &str_row,
    map<vector<T>, value_type> &cambio
){
    // Límite de superficie con cambio de especie
    string str_p, str_r, str_z, str_s, str_m, str_n, str_j;
    string rest, rhi;
    float r_value;

    for (unsigned short p : P) {
        str_p = index_string(p);
        for (unsigned short r : R) {
            str_r = index_string(r);
            for (unsigned short z : Z) {
                str_z = index_string(z);
                for (unsigned short s : S) {
                    str_s = index_string(s);
                    for (unsigned short m : M) {
                        str_m = index_string(m);
                        for (unsigned short n : M) {
                            if (m == n) continue; // se prohiben las rest. TT para m == n
                            str_n = index_string(n);

                            r_value = cambio[{p, r, z, s, m, n}];
                            
                            rest = "TT" + str_p + str_r + str_z + str_s + str_m + str_n;
                            str_rest += rest + ": ";
                            rhi = format(r_value);

                            for (unsigned short j = 0; j <= 9; j++) {
                                str_j = index_string(j);
                                str_rest += "T" + str_p + str_r + str_z + str_s + str_m + str_n + str_j + \
                                    (j != 9 ? " + " : "");
                            }

                            str_rest += " <= " + rhi + " ";
                            str_row += rest + ",L," + rhi + "\n";
                            
                        }
                    }
                }
            }
        }
    }
}



void constraint7(
    string& str_rest, 
    string& str_col,
    string& str_row
){
    // Reforestación  A (superficie disponible para reforestar con o sin cambio de especie)
    string str_p, str_r, str_z, str_s, str_m, str_k, str_i, str_j, str_n;
    string rest;

    for (unsigned short p : P) {
        str_p = index_string(p);
        for (unsigned short r : R){
            str_r = index_string(r);
            for (unsigned short z : Z){
                str_z = index_string(z);
                for (unsigned short s : S){
                    str_s = index_string(s);
                    for (unsigned short m : M){
                        str_m = index_string(m);
                        for (unsigned short j = 0; j <= HP; j++){
                            str_j = index_string(j);

                            rest = "RFA" + str_p + str_r + str_z + str_s + str_m + str_j;
                            str_rest += rest + ": ";

                            str_rest += "SST" + str_p + str_r + str_z + str_s + str_m + str_j;
                            
                            if (j < 10) {
                                for (unsigned short n : M) {
                                    str_n = index_string(n);
                                    str_rest += " - T" + str_p + str_r + str_z + str_s + str_m + str_n + str_j;
                                }
                            }
                            else { // para 10 <= j <= 45 solo aparecen Tprzsmnj para m == n
                                str_rest += " - T" + str_p + str_r + str_z + str_s + str_m + str_m + str_j;
                            }

                            str_rest += " = 0 ";
                            str_row += rest + ",E,0\n";
                        }
                    }
                }
            }
        }
    }
}



template <typename T, typename value_type> void constraint8(
    string& str_rest, 
    string& str_col,
    string& str_row,
    map<vector<T>, value_type>& cambio
){
    // Reforestación  B (superficie a reforestar con una especie igual o distinta a la anterior)
    string str_p, str_r, str_z, str_s, str_m, str_j, str_l, str_k;
    string str_temp, rest;
    float value;

    vector<unsigned short> rg;

    for (unsigned short p : P) {
        str_p = index_string(p);
        for (unsigned short r : R) {
            str_r = index_string(r);
            for (unsigned short z : Z) {
                str_z = index_string(z);
                for (unsigned short s : S) {
                    str_s = index_string(s);
                    for (unsigned short m : M) {
                        str_m = index_string(m);

                        rg = get_range<unsigned short>(p, m);

                        for (unsigned short j = 0; j <= HP; j++) {
                            str_j = index_string(j);

                            rest = "RFB" + str_p + str_r + str_z + str_s + str_m + str_j;
                            str_rest += rest + ": ";
                            str_temp = "";

                            // sumatoria 1: l / (p,r,z,s,l,m)
                            if (j <= 10) {
                                for (unsigned short l : M) {
                                    str_l = index_string(l);

                                    value = cambio[{p, r, z, s, l, m}];

                                    if (value > 1e-3 || l == m) { 
                                        str_temp += "T" + str_p + str_r + str_z + str_s + str_l + str_m + str_j + " + ";
                                    }
                                }
                            }
                            else { // para 10 < j <= 45 solo aparecen Tprzsmnj para m == l
                                str_temp += "T" + str_p + str_r + str_z + str_s + str_m + str_m + str_j + " + ";
                            }

                            // variable Fprszsmj (solo para j == 0 o j == 1)
                            if (j <= 1) {
                                str_temp += "F" + str_p + str_r + str_z + str_s + str_m + str_j;
                            }
                            else { // se saca el " + "; siempre hay un T
                                str_temp = str_temp.substr(0, str_temp.length() - 3);
                            }
                            str_rest += str_temp;

                            // sumatoria 2: k = j+edminm -> j+edmaxm
                            for (unsigned short k = j + rg[0]; k <= min(HP, j + rg[1]); k++) {
                                str_k = index_string(k);

                                str_rest += " - X" + str_p + str_r + str_z + str_s + str_m + str_j + str_k;
                            }

                            // variable Wprzsmj
                            if (j + rg[1] >= HP) { // solo si la plantacion + edad maxima es >= HP
                                str_rest += " - W" + str_p + str_r + str_z + str_s + str_m + str_j;
                            }

                            // lado derecho
                            str_rest += " = 0 ";
                            str_row += rest + ",E,0\n";
                        }
                    }
                }
            }
        }
    }
}



template <typename T, typename value_type> void constraint9(
    string& str_rest, 
    string& str_col,
    string& str_row,
    map<vector<T>, value_type>& raleo
){
    // Cuantificación de volumen de raleos
    string str_k, str_p, str_r, str_m, str_j, str_z, str_s, str_n, str_jl, str_plt;
    string str_temp, rest, var, str_val;
    short plt;
    float value;
    bool changes;

    vector<unsigned short> rg;

    for (unsigned short k = 4; k <= 7; k++) {
        str_k = index_string(k);
        for (unsigned short p : P) {
            str_p = index_string(p);
            for (unsigned short r : R) {
                str_r = index_string(r);
                for (unsigned short m : M) {
                    str_m = index_string(m);

                    rg = get_range(p, m);

                    for (unsigned short j = 0; j <= HP; j++) {
                        str_j = index_string(j);

                        rest = "RT" + str_k + str_p + str_r + str_m + str_j;
                        str_rest += rest + ": ";
                        changes = false;

                        // sumatorias para Y X W
                        str_temp = "";
                        for (unsigned short z : Z) {
                            str_z = index_string(z);
                            for (unsigned short s : S) {
                                str_s = index_string(s);
                                for (unsigned short jl = 9; jl <= 19; jl++) {
                                    str_jl = index_string(jl);

                                    value = raleo[{k, z, s, m, jl}];

                                    if (value > 1e-3) {
                                        str_val = format(value);
                                        plt = (short)j - (short)jl;
                                        str_plt = index_string(plt < 0 ? -plt : plt);
                                        for (unsigned short n = max(0, plt + rg[0]); n <= min(HP, plt + rg[1]); n++) {
                                            str_n = index_string(n);
                                            var = (plt < 0 ? "Y" : "X") + str_p + str_r + str_z + str_s + str_m + str_plt + str_n;
                                            str_temp += str_val + " " + var + " + ";
                                            str_col += "RT," + var + "," + str_val + "\n";
                                        }
                                        if (plt + rg[1] >= HP) { // inventario final
                                            var = "W" + str_p + str_r + str_z + str_s + str_m + str_plt;
                                            str_temp += str_val + " " + var + " + ";
                                            str_col += "RT," + var + "," + str_val + "\n";
                                        }
                                    }
                                }
                            }
                        }

                        if (str_temp.length() > 3) { // añade cambios, si es que hay
                            str_rest += str_temp.substr(0, str_temp.length() - 3);
                            changes = true;
                        }

                        // variable Rkprmj
                        str_rest += (changes ? " - R" : "R") + str_k + str_p + str_r + str_m + str_j;

                        // lado derecho
                        str_rest += " = 0 ";
                        str_row += rest + ",E,0\n";
                    }
                }
            }
        }
    }
}


template <typename T, typename value_type> void constraint10(
    string& str_rest,
    string& str_col,
    string& str_row,
    map<vector<T>, value_type>& fores
) {
    // Superficie disponible para forestación
    string str_p, str_r, str_z, str_s, str_m, str_j;
    string rest, rhi;

    for (unsigned short p : P) {
        str_p = index_string(p);
        for (unsigned short r : R) {
            str_r = index_string(r);
            for (unsigned short z : Z) {
                str_z = index_string(z);
                for (unsigned short s : S) {
                    str_s = index_string(s);
                    for (unsigned short m : M) {
                        str_m = index_string(m);
                        for (unsigned short j = 0; j <= 1; j++) {
                            str_j = index_string(j);

                            rest = "FT" + str_p + str_r + str_z + str_s + str_m + str_j;
                            str_rest += rest + ": ";

                            // variable Fprzsmj
                            str_rest += "F" + str_p + str_r + str_z + str_s + str_m + str_j;

                            // lado derecho
                            rhi = format(fores[{p, r, z, s, m, j}]);
                            str_rest += " <= " + rhi + " ";
                            str_row += rest + ",L," + rhi + "\n";
                        }
                    }
                }
            }
        }
    }
}