#pragma once
// Datos de prueba y medición de tiempo, nodos revisados y memoria.
// Lo usan las cuatro aplicaciones, así las cifras son comparables entre ellas.

#include <chrono>
#include <random>
#include <string>
#include <vector>

#include "libro.h"
#include "memoria.h"

using namespace std;
typedef chrono::steady_clock Reloj;

// 100 casillas iniciales de la tabla * 2^20. Todos sus múltiplos caen en la casilla 0
// mientras la tabla tenga menos de 2^20 duplicaciones: el peor caso de la hash.
const long long SALTO_ADVERSARIO = 104857600LL;

// Por encima de este N no se corren los casos O(n^2) (BST ordenado, hash adversario):
// tardarían minutos y la recursión del árbol degenerado podría desbordar la pila.
const int LIMITE_CUADRATICO = 20000;

const int BUSQUEDAS = 10000;
volatile long long sumidero = 0;   // Evita que el compilador elimine las búsquedas

enum Escenario { ALEATORIO, ORDENADO, ADVERSARIO, NINGUNO };
const char* NOMBRE_ESCENARIO[] = {
    "ALEATORIO  - codigos en desorden (caso promedio)",
    "ORDENADO   - codigos de menor a mayor, como el Excel (peor caso del BST)",
    "ADVERSARIO - codigos que colisionan en la hash (peor caso de la hash)"};

// ---------------------------------------------------------------- datos de prueba

struct Datos {
    vector<Libro> libros;              // En el orden en que se insertan
    vector<long long> existentes;      // Códigos a buscar que SÍ están
    vector<long long> inexistentes;    // Códigos a buscar que NO están
};

Libro libro_sintetico(long long codigo) {
    Libro l;
    l.codigo = codigo;
    l.titulo = "Libro " + to_string(codigo);
    l.editorial = "Editorial " + to_string(codigo % 40);
    l.autores = "Autor " + to_string(codigo % 300);
    return l;
}

Datos generar_datos(int n, Escenario escenario) {
    mt19937_64 aleatorio(2026 + n * 3 + escenario);
    vector<long long> codigos(n), faltantes(n);
    for (int i = 0; i < n; i++) {
        if (escenario == ADVERSARIO) {
            codigos[i] = (long long)(i + 1) * SALTO_ADVERSARIO;
            faltantes[i] = (long long)(n + i + 1) * SALTO_ADVERSARIO;
        } else {
            codigos[i] = 101 + 3LL * i;     // 101, 104, 107...
            faltantes[i] = 102 + 3LL * i;   // 102, 105, 108... (huecos entre los existentes)
        }
    }
    if (escenario != ORDENADO) {
        for (int i = n - 1; i > 0; i--) {   // Barajar (Fisher-Yates)
            int j = aleatorio() % (i + 1);
            long long temporal = codigos[i];
            codigos[i] = codigos[j];
            codigos[j] = temporal;
        }
    }

    Datos datos;
    for (int i = 0; i < n; i++) datos.libros.push_back(libro_sintetico(codigos[i]));
    int m = n < BUSQUEDAS ? n : BUSQUEDAS;
    for (int i = 0; i < m; i++) {
        datos.existentes.push_back(codigos[aleatorio() % n]);
        datos.inexistentes.push_back(faltantes[aleatorio() % n]);
    }
    return datos;
}

// ---------------------------------------------------------------- medición

struct Resultado {
    bool omitido = false;
    bool error = false;
    double ms_construir = 0;     // Insertar los N libros
    double ns_buscar_ok = 0;     // Promedio por búsqueda de un código que existe
    double ns_buscar_fallo = 0;  // Promedio por búsqueda de un código que no existe
    double ms_listar = 0;        // Sacar todos los libros ordenados por código
    double comp_ok = 0;          // Nodos revisados en promedio por búsqueda exitosa
    double comp_fallo = 0;
    double kb_estructura = 0;    // Memoria que ocupa la estructura ya construida
    double bytes_por_libro = 0;  // kb_estructura / N (incluye el libro guardado)
    double kb_pico_insertar = 0; // Máximo de memoria mientras se insertaba
    double kb_extra_listar = 0;  // Búfer extra al listar, NO incluye arreglo de salida común
    string forma;
};

double milisegundos(Reloj::time_point desde, Reloj::time_point hasta) {
    return chrono::duration<double, milli>(hasta - desde).count();
}

template <class Estructura>
Resultado medir(const Datos& datos, int repeticiones) {
    Resultado r;
    int n = datos.libros.size();
    double busquedas = datos.existentes.size();
    // Arreglo común de salida: 8N bytes en el entorno de 64 bits; fuera de "extra listar".
    Libro** lista = new Libro*[n];

    for (int rep = 0; rep < repeticiones; rep++) {
        long long memoria_inicial = memoria::en_uso;
        memoria::reiniciar_pico();
        Estructura estructura;

        Reloj::time_point t0 = Reloj::now();
        for (int i = 0; i < n; i++) estructura.insertar(datos.libros[i].codigo, datos.libros[i]);
        Reloj::time_point t1 = Reloj::now();
        long long bytes_estructura = memoria::en_uso - memoria_inicial;
        long long bytes_pico = memoria::pico - memoria_inicial;

        estructura.comparaciones = 0;
        long long encontrados = 0;
        for (long long codigo : datos.existentes)
            if (estructura.buscar(codigo) != nullptr) encontrados++;
        Reloj::time_point t2 = Reloj::now();
        long long comp_ok = estructura.comparaciones;

        estructura.comparaciones = 0;
        long long falsos = 0;
        for (long long codigo : datos.inexistentes)
            if (estructura.buscar(codigo) != nullptr) falsos++;
        Reloj::time_point t3 = Reloj::now();
        long long comp_fallo = estructura.comparaciones;

        long long memoria_antes_listar = memoria::en_uso;
        memoria::reiniciar_pico();
        Reloj::time_point t3b = Reloj::now();
        int pos = 0;
        estructura.recorrido_inorden(lista, pos);
        Reloj::time_point t4 = Reloj::now();
        long long bytes_listar = memoria::pico - memoria_antes_listar;

        // Comprobación de que la estructura respondió bien
        if (encontrados != (long long)busquedas || falsos != 0 || pos != n) r.error = true;
        for (int i = 1; i < pos; i++)
            if (lista[i - 1]->codigo >= lista[i]->codigo) r.error = true;
        sumidero = sumidero + encontrados + pos;

        r.ms_construir += milisegundos(t0, t1);
        r.ns_buscar_ok += milisegundos(t1, t2) * 1e6 / busquedas;
        r.ns_buscar_fallo += milisegundos(t2, t3) * 1e6 / busquedas;
        r.ms_listar += milisegundos(t3b, t4);
        r.comp_ok += comp_ok / busquedas;
        r.comp_fallo += comp_fallo / busquedas;
        // La memoria no varía entre repeticiones: basta con la última
        r.kb_estructura = bytes_estructura / 1024.0;
        r.bytes_por_libro = (double)bytes_estructura / n;
        r.kb_pico_insertar = bytes_pico / 1024.0;
        r.kb_extra_listar = bytes_listar / 1024.0;
        r.forma = estructura.forma();
    }
    delete[] lista;

    r.ms_construir /= repeticiones;
    r.ns_buscar_ok /= repeticiones;
    r.ns_buscar_fallo /= repeticiones;
    r.ms_listar /= repeticiones;
    r.comp_ok /= repeticiones;
    r.comp_fallo /= repeticiones;
    return r;
}

// "peor" es el escenario que vuelve O(n^2) a la estructura (NINGUNO para el AVL)
template <class Estructura>
Resultado medir_si_cabe(const Datos& datos, Escenario escenario, Escenario peor, int repeticiones) {
    if (escenario == peor && (int)datos.libros.size() > LIMITE_CUADRATICO) {
        Resultado r;
        r.omitido = true;
        return r;
    }
    return medir<Estructura>(datos, repeticiones);
}
