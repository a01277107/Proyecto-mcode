#include <iostream>
#include <fstream>
#include <string>
#include <vector>
#include <algorithm>

using namespace std;

// Lee un archivo y devuelve solo los caracteres validos (0-9, A-F),
// ignorando saltos de linea, retornos de carro y cualquier espacio.

string leerArchivoLimpio(const string &nombreArchivo) {
    ifstream archivo(nombreArchivo);
    string resultado;

    if (!archivo.is_open()) {
        cerr << "Error: no se pudo abrir el archivo " << nombreArchivo << endl;
        return resultado;
    }

    char c;
    while (archivo.get(c)) {
        // Solo conservamos digitos 0-9 y letras A-F (mayus o minus, por seguridad)
        if ((c >= '0' && c <= '9') ||
            (c >= 'A' && c <= 'F') ||
            (c >= 'a' && c <= 'f')) {
            resultado += static_cast<char>(toupper(static_cast<unsigned char>(c)));
        }
        // saltos de linea, \r y cualquier otro caracter se ignoran
    }

    archivo.close();
    return resultado;
}


// PARTE 1: busca "patron" dentro de "texto".
// Devuelve la posicion 1-based donde inicia, o -1 si no se encuentra.
// Usamos el algoritmo KMP para eficiencia en archivos grandes.

vector<int> construirTablaFallo(const string &patron) {
    int m = patron.size();
    vector<int> fallo(m, 0);
    int len = 0;
    int i = 1;
    while (i < m) {
        if (patron[i] == patron[len]) {
            len++;
            fallo[i] = len;
            i++;
        } else {
            if (len != 0) {
                len = fallo[len - 1];
            } else {
                fallo[i] = 0;
                i++;
            }
        }
    }
    return fallo;
}

long long buscarPatronKMP(const string &texto, const string &patron) {
    if (patron.empty() || texto.size() < patron.size()) return -1;

    vector<int> fallo = construirTablaFallo(patron);
    int n = texto.size();
    int m = patron.size();
    int i = 0; // indice en texto
    int j = 0; // indice en patron

    while (i < n) {
        if (texto[i] == patron[j]) {
            i++;
            j++;
            if (j == m) {
                // Encontrado: posicion de inicio (0-based) = i - j
                return (i - j) + 1; // convertir a 1-based
            }
        } else {
            if (j != 0) {
                j = fallo[j - 1];
            } else {
                i++;
            }
        }
    }
    return -1;
}


// PARTE 2: palindromo (codigo "espejeado") mas largo dentro de una cadena.
// Algoritmo de Manacher, O(n).
// Devuelve un par (inicio, fin) en posiciones 1-based (ambas inclusive).

pair<int,int> palindromoMasLargo(const string &s) {
    int n = s.size();
    if (n == 0) return {0, 0};

    // Construimos cadena transformada con separadores '#'
    string t;
    t.reserve(2 * n + 1);
    t += '#';
    for (char c : s) {
        t += c;
        t += '#';
    }

    int tn = t.size();
    vector<int> p(tn, 0);
    int center = 0, right = 0;

    for (int i = 0; i < tn; i++) {
        int mirror = 2 * center - i;
        if (i < right) {
            p[i] = min(right - i, p[mirror]);
        }
        while (i - p[i] - 1 >= 0 && i + p[i] + 1 < tn &&
               t[i - p[i] - 1] == t[i + p[i] + 1]) {
            p[i]++;
        }
        if (i + p[i] > right) {
            center = i;
            right = i + p[i];
        }
    }

    int maxLen = 0, centerIndex = 0;
    for (int i = 0; i < tn; i++) {
        if (p[i] > maxLen) {
            maxLen = p[i];
            centerIndex = i;
        }
    }

    // Posicion inicial 0-based en la cadena original
    int startZeroBased = (centerIndex - maxLen) / 2;
    int lengthPal = maxLen;
    int endZeroBased = startZeroBased + lengthPal - 1;

    return {startZeroBased + 1, endZeroBased + 1}; // convertir a 1-based
}

// PARTE 3: substring comun mas largo entre dos cadenas.
// Programacion dinamica con dos filas (ahorro de memoria).
// Devuelve posiciones 1-based (inicio, fin) referidas a la PRIMERA cadena.

pair<int,int> substringComunMasLargo(const string &s1, const string &s2) {
    int n = s1.size();
    int m = s2.size();

    if (n == 0 || m == 0) return {0, 0};

    vector<int> filaAnterior(m + 1, 0);
    vector<int> filaActual(m + 1, 0);

    int maxLen = 0;
    int finEnS1 = -1; // indice 0-based del ultimo caracter del match en s1

    for (int i = 1; i <= n; i++) {
        for (int j = 1; j <= m; j++) {
            if (s1[i - 1] == s2[j - 1]) {
                filaActual[j] = filaAnterior[j - 1] + 1;
                if (filaActual[j] > maxLen) {
                    maxLen = filaActual[j];
                    finEnS1 = i - 1; // 0-based
                }
            } else {
                filaActual[j] = 0;
            }
        }
        filaAnterior = filaActual;
    }

    if (maxLen == 0) return {0, 0};

    int inicioEnS1 = finEnS1 - maxLen + 1;
    return {inicioEnS1 + 1, finEnS1 + 1}; // 1-based
}

// MAIN

int main() {
    // Nombres fijos de archivos
    string archTrans1 = "transmission01.txt";
    string archTrans2 = "transmission02.txt";
    string archMcode1 = "mcode01.txt";
    string archMcode2 = "mcode02.txt";
    string archMcode3 = "mcode03.txt";

    string trans1 = leerArchivoLimpio(archTrans1);
    string trans2 = leerArchivoLimpio(archTrans2);
    string mcode1 = leerArchivoLimpio(archMcode1);
    string mcode2 = leerArchivoLimpio(archMcode2);
    string mcode3 = leerArchivoLimpio(archMcode3);

    vector<string> transmisiones = {trans1, trans2};
    vector<string> mcodes = {mcode1, mcode2, mcode3};

//Parte 1
    for (size_t t = 0; t < transmisiones.size(); t++) {
        for (size_t c = 0; c < mcodes.size(); c++) {
            long long pos = buscarPatronKMP(transmisiones[t], mcodes[c]);
            if (pos != -1) {
                cout << "true " << pos << endl;
            } else {
                cout << "false" << endl;
            }
        }
    }

//Parte 2
    for (size_t t = 0; t < transmisiones.size(); t++) {
        pair<int,int> res = palindromoMasLargo(transmisiones[t]);
        cout << res.first << " " << res.second << endl;
    }

//Parte 3
    pair<int,int> comun = substringComunMasLargo(trans1, trans2);
    cout << comun.first << " " << comun.second << endl;

    return 0;
}