#pragma once
#include "libro.h"

// ALGORITMO 1: Árbol Binario de Búsqueda SIN balancear (el mismo de la app en Python).
// Paradigma: Reducir y vencerás (cada comparación descarta un subárbol).
// Clave = código del libro. Menores a la izquierda, mayores a la derecha.

class NodoArbol {
public:
    long long clave;      // El codigo del libro
    Libro valor;          // El libro completo
    NodoArbol* izquierdo;
    NodoArbol* derecho;

    NodoArbol(long long c, const Libro& v) {
        clave = c;
        valor = v;
        izquierdo = nullptr;
        derecho = nullptr;
    }
};

class ArbolBinarioBusqueda {
public:
    NodoArbol* raiz;
    long long comparaciones;   // Cuántos nodos se han revisado (para el análisis)

    ArbolBinarioBusqueda() {
        raiz = nullptr;
        comparaciones = 0;
    }

    ~ArbolBinarioBusqueda() {
        liberar(raiz);
    }

    void insertar(long long clave, const Libro& valor) {
        if (raiz == nullptr) {
            raiz = new NodoArbol(clave, valor);
            return;
        }
        NodoArbol* actual = raiz;
        while (true) {
            comparaciones++;
            if (clave < actual->clave) {
                if (actual->izquierdo == nullptr) {
                    actual->izquierdo = new NodoArbol(clave, valor);
                    return;
                }
                actual = actual->izquierdo;
            } else if (clave > actual->clave) {
                if (actual->derecho == nullptr) {
                    actual->derecho = new NodoArbol(clave, valor);
                    return;
                }
                actual = actual->derecho;
            } else {
                // Si la clave ya existe, se actualiza el libro
                actual->valor = valor;
                return;
            }
        }
    }

    Libro* buscar(long long clave) {
        NodoArbol* actual = raiz;
        while (actual != nullptr) {
            comparaciones++;
            if (clave == actual->clave) {
                return &actual->valor;
            }
            if (clave < actual->clave) {
                actual = actual->izquierdo;
            } else {
                actual = actual->derecho;
            }
        }
        return nullptr;
    }

    // Deja en "resultado" los libros ordenados por código (izquierda, raíz, derecha)
    void recorrido_inorden(Libro** resultado, int& pos) {
        inorden_recursivo(raiz, resultado, pos);
    }

    int altura() {
        return altura_recursiva(raiz);
    }

    string forma() {
        return "altura=" + to_string(altura());
    }

private:
    void inorden_recursivo(NodoArbol* nodo, Libro** resultado, int& pos) {
        if (nodo == nullptr) return;
        inorden_recursivo(nodo->izquierdo, resultado, pos);
        resultado[pos] = &nodo->valor;
        pos++;
        inorden_recursivo(nodo->derecho, resultado, pos);
    }

    int altura_recursiva(NodoArbol* nodo) {
        if (nodo == nullptr) return 0;
        int izq = altura_recursiva(nodo->izquierdo);
        int der = altura_recursiva(nodo->derecho);
        if (izq > der) return izq + 1;
        return der + 1;
    }

    void liberar(NodoArbol* nodo) {
        if (nodo == nullptr) return;
        liberar(nodo->izquierdo);
        liberar(nodo->derecho);
        delete nodo;
    }
};
