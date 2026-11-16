#pragma once
#include <cstdio>
#include "libro.h"

// ALGORITMO 3: Tabla Hash con encadenamiento (la misma idea de la de usuarios en Python).
// casilla = codigo % tamano. Si dos libros caen en la misma casilla, se encadenan
// en una lista enlazada. Cuando el factor de carga pasa de 0.7, la tabla se duplica.

class NodoHash {
public:
    long long clave;
    Libro valor;
    NodoHash* siguiente;   // Para encadenamiento en caso de colisión

    NodoHash(long long c, const Libro& v) {
        clave = c;
        valor = v;
        siguiente = nullptr;
    }
};

class TablaHash {
public:
    NodoHash** tabla;
    long long tamano;
    long long num_elementos;
    long long comparaciones;

    TablaHash() {
        tamano = 100;
        num_elementos = 0;
        comparaciones = 0;
        tabla = new NodoHash*[tamano];
        for (long long i = 0; i < tamano; i++) tabla[i] = nullptr;
    }

    ~TablaHash() {
        for (long long i = 0; i < tamano; i++) {
            NodoHash* actual = tabla[i];
            while (actual != nullptr) {
                NodoHash* siguiente = actual->siguiente;
                delete actual;
                actual = siguiente;
            }
        }
        delete[] tabla;
    }

    long long hash(long long clave) {
        return clave % tamano;
    }

    void insertar(long long clave, const Libro& valor) {
        long long indice = hash(clave);

        // Revisar si la clave ya existe para actualizar el libro
        NodoHash* actual = tabla[indice];
        while (actual != nullptr) {
            comparaciones++;
            if (actual->clave == clave) {
                actual->valor = valor;
                return;
            }
            actual = actual->siguiente;
        }

        // Insertar al inicio de la cadena
        NodoHash* nuevo = new NodoHash(clave, valor);
        nuevo->siguiente = tabla[indice];
        tabla[indice] = nuevo;
        num_elementos++;

        // Redimensionar si el factor de carga pasa de 0.7
        if (num_elementos * 10 > tamano * 7) {
            redimensionar();
        }
    }

    Libro* buscar(long long clave) {
        NodoHash* actual = tabla[hash(clave)];
        while (actual != nullptr) {
            comparaciones++;
            if (actual->clave == clave) {
                return &actual->valor;
            }
            actual = actual->siguiente;
        }
        return nullptr;
    }

    // La tabla NO guarda orden: hay que recoger todos los libros y ordenarlos.
    void recorrido_inorden(Libro** resultado, int& pos) {
        for (long long i = 0; i < tamano; i++) {
            NodoHash* actual = tabla[i];
            while (actual != nullptr) {
                resultado[pos] = &actual->valor;
                pos++;
                actual = actual->siguiente;
            }
        }
        Libro** auxiliar = new Libro*[pos];
        ordenar_por_mezcla(resultado, auxiliar, 0, pos);
        delete[] auxiliar;
    }

    string forma() {
        long long cadena_max = 0;
        for (long long i = 0; i < tamano; i++) {
            long long largo = 0;
            NodoHash* actual = tabla[i];
            while (actual != nullptr) {
                largo++;
                actual = actual->siguiente;
            }
            if (largo > cadena_max) cadena_max = largo;
        }
        char texto[80];
        snprintf(texto, sizeof(texto), "carga=%.2f cadena_max=%lld",
                 (double)num_elementos / tamano, cadena_max);
        return texto;
    }

private:
    void redimensionar() {
        long long tamano_viejo = tamano;
        NodoHash** tabla_vieja = tabla;

        tamano = tamano * 2;
        tabla = new NodoHash*[tamano];
        for (long long i = 0; i < tamano; i++) tabla[i] = nullptr;

        // Mover cada nodo a su nueva casilla
        for (long long i = 0; i < tamano_viejo; i++) {
            NodoHash* actual = tabla_vieja[i];
            while (actual != nullptr) {
                NodoHash* siguiente = actual->siguiente;
                long long indice = hash(actual->clave);
                actual->siguiente = tabla[indice];
                tabla[indice] = actual;
                actual = siguiente;
            }
        }
        delete[] tabla_vieja;
    }

    // Merge sort sobre [inicio, fin): O(n log n)
    void ordenar_por_mezcla(Libro** a, Libro** aux, int inicio, int fin) {
        if (fin - inicio < 2) return;
        int mitad = (inicio + fin) / 2;
        ordenar_por_mezcla(a, aux, inicio, mitad);
        ordenar_por_mezcla(a, aux, mitad, fin);

        int i = inicio, j = mitad, k = inicio;
        while (i < mitad && j < fin) {
            if (a[i]->codigo <= a[j]->codigo) aux[k++] = a[i++];
            else aux[k++] = a[j++];
        }
        while (i < mitad) aux[k++] = a[i++];
        while (j < fin) aux[k++] = a[j++];
        for (k = inicio; k < fin; k++) a[k] = aux[k];
    }
};
