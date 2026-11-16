#pragma once
// Utilidades de consola compartidas: leer números, pausas, rutas, leer el CSV del
// catálogo, mostrar el código de un archivo y probar una búsqueda.

#include <cstdio>
#include <cstdlib>
#include <fstream>
#include <iostream>
#include <string>
#include <vector>

#include "libro.h"
#include "medicion.h"

#ifdef _WIN32
#define WIN32_LEAN_AND_MEAN
#define NOMINMAX
#include <windows.h>
#endif

using namespace std;

string carpeta_raiz;   // Raíz del proyecto (donde están comun/ y datos/)

// Cada ejecutable vive en su carpeta (1_comparar_todas/, 2_bst/...): la raíz es la de arriba
void preparar_consola(const char* argv0) {
#ifdef _WIN32
    SetConsoleOutputCP(CP_UTF8);
#endif
    string ejecutable = argv0;
    size_t barra = ejecutable.find_last_of("/\\");
    string carpeta = barra == string::npos ? "" : ejecutable.substr(0, barra + 1);
    carpeta_raiz = carpeta + "../";
}

int leer_entero(const string& mensaje, int minimo, int maximo) {
    while (true) {
        cout << mensaje;
        string linea;
        if (!getline(cin, linea)) exit(0);
        try {
            size_t usados = 0;
            long long valor = stoll(linea, &usados);
            if (usados == linea.size() && valor >= minimo && valor <= maximo) return (int)valor;
        } catch (...) {
        }
        cout << "  Valor invalido: escribe un numero entre " << minimo << " y " << maximo << ".\n";
    }
}

void pausa() {
    cout << "\nPresiona Enter para volver al menu...";
    string linea;
    getline(cin, linea);
}

int pedir_escenario() {
    cout << "\nEscenario:\n";
    for (int e = 0; e < 3; e++) cout << "  " << e + 1 << ") " << NOMBRE_ESCENARIO[e] << "\n";
    cout << "  4) Los tres escenarios\n";
    return leer_entero("Opcion: ", 1, 4) - 1;
}

vector<Libro> cargar_catalogo(const string& ruta) {
    vector<Libro> libros;
    ifstream archivo(ruta);
    string linea;
    getline(archivo, linea);   // Encabezado
    while (getline(archivo, linea)) {
        if (!linea.empty() && linea.back() == '\r') linea.pop_back();
        if (linea.empty()) continue;
        string campos[4];
        int c = 0;
        for (char letra : linea) {
            if (letra == ';' && c < 3) c++;
            else campos[c] += letra;
        }
        try {
            Libro l;
            l.codigo = stoll(campos[0]);
            l.titulo = campos[1];
            l.editorial = campos[2];
            l.autores = campos[3];
            libros.push_back(l);
        } catch (...) {
        }
    }
    return libros;
}

string ruta_catalogo() {
    return carpeta_raiz + "datos/libros.csv";
}

void mostrar_archivo(const string& nombre) {
    string ruta = carpeta_raiz + "comun/" + nombre;
    ifstream archivo(ruta);
    if (!archivo) {
        cout << "No se encontro " << ruta << "\n";
        return;
    }
    cout << "\n";
    string linea;
    int numero = 1;
    while (getline(archivo, linea)) printf("%4d | %s\n", numero++, linea.c_str());
}

template <class Estructura>
void probar_busqueda(const char* nombre, Estructura& estructura, long long codigo) {
    estructura.comparaciones = 0;
    Libro* libro = estructura.buscar(codigo);
    long long nodos = estructura.comparaciones;

    // Una sola búsqueda dura nanosegundos: se repite muchas veces y se promedia
    const int VECES = 200000;
    Reloj::time_point t0 = Reloj::now();
    for (int i = 0; i < VECES; i++) sumidero = sumidero + (estructura.buscar(codigo) != nullptr);
    Reloj::time_point t1 = Reloj::now();
    double ns = milisegundos(t0, t1) * 1e6 / VECES;

    printf("  %-11s %7.1f ns  %3lld nodos revisados  -> %s\n", nombre, ns, nodos,
           libro ? (libro->titulo + " (" + libro->autores + ")").c_str() : "no esta en el catalogo");
}
